#include "pluginmanager.h"
#include <QMap>
#include <QList>
#include <QLibrary>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtDebug>
#include <QJsonArray>
#include <atomic>
#include "iplugin.h"

namespace exsys{

class PluginSpec: public IPluginSpec
{
public:
    typedef void (*FInitPlugin)();
    typedef void (*FUninitPlugin)();
    typedef int (*FQueryObject)(const char *, void **);
    typedef const char *(*FGetPluginInfo)();

    PluginSpec(const std::string& path):
        InitPlugin_(nullptr)
      , UninitPlugin_(nullptr)
      , QueryObjects_(nullptr)
      , GetPluginInfo_(nullptr)
      , is_started_(false)
      , ref_(0)
    {
        lib_.setFileName(QString::fromStdString(path));
    }

    bool Load()
    {
        qDebug() << "loading " << GetPluginFilePath().data() << " ...";

        if(!lib_.load()){
            error_string_ = lib_.errorString().toStdString();
            Q_ASSERT(false);
            return false;
        }

#define INIT_SYMBOL "InitializePlugin"
#define UNINIT_SYMBOL "UninitializePlugin"
#define QUEYR_SYMBOL "QueryPluginObjects"
#define GET_INFO_SYMBOL "GetPluginInfo"
        InitPlugin_ = (FInitPlugin)lib_.resolve(INIT_SYMBOL);
        if(nullptr == InitPlugin_){
            error_string_ = lib_.errorString().toStdString();
            Q_ASSERT(false);
            return false;
        }

        UninitPlugin_ = (FUninitPlugin)lib_.resolve(UNINIT_SYMBOL);
        if(nullptr == UninitPlugin_){
            error_string_ = lib_.errorString().toStdString();
            Q_ASSERT(false);
            return false;
        }

        QueryObjects_ = (FQueryObject)lib_.resolve(QUEYR_SYMBOL);
        if(nullptr == QueryObjects_){
            error_string_ = lib_.errorString().toStdString();
            Q_ASSERT(false);
            return false;
        }

        GetPluginInfo_ = (FGetPluginInfo)lib_.resolve(GET_INFO_SYMBOL);
        if(nullptr == GetPluginInfo_){
            error_string_ = lib_.errorString().toStdString();
            Q_ASSERT(false);
            return false;
        }

        InitPlugin_();

        const char *plugin_info = GetPluginInfo_();
        if(plugin_info != nullptr){
            ParsePluginInfo(plugin_info);
        }

        iplugin_objs_.clear();
        std::vector<void *> objs;
        QueryObjects(IPLUGIN_IID, objs);
        for(void *obj: objs){
            iplugin_objs_.push_back(static_cast<IPlugin *>(obj));
        }

        return true;
    }

    int QueryObjects(const std::string &iid, std::vector<void *> &objs)
    {
        int obj_count = QueryObjects_(iid.c_str(), nullptr);
        objs.clear();
        if(obj_count > 0){
            objs.resize(obj_count);
            obj_count = QueryObjects_(iid.c_str(), objs.data());
        }

        return obj_count;
    }

    bool IsSupportInterface(const std::string &iid)
    {
        return QueryObjects_(iid.c_str(), nullptr) > 0;
    }

    bool Unload()
    {
        qDebug() << "unloading " << GetPluginFilePath().data() << " ...";

        if(UninitPlugin_ != nullptr){
            UninitPlugin_ ();
        }
        else{
            Q_ASSERT(false);
        }

        if(!lib_.isLoaded()){
            error_string_ = "not loaded!";
            return false;
        }

        lib_.unload();
        return true;
    }

    bool ParsePluginInfo(const std::string& info)
    {
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromStdString(info), &error);
        if(error.error == QJsonParseError::NoError){
            Q_ASSERT(jsonDoc.isObject());
            QJsonObject jobj = jsonDoc.object();

            for(QString key: jobj.keys()){
                plugin_info_dict_[key.toStdString()] = jobj[key].toString().toStdString();
            }
        }
        else{
            error_string_ = error.errorString().toStdString();
            Q_ASSERT(false);
            return false;
        }

        return true;
    }

    bool IsStarted()
    {
        return is_started_;
    }

    bool Startup()
    {
        qDebug() << "starting: " << GetPluginFilePath().data() << " ...";
        for(IPlugin *val: iplugin_objs_){
            std::string err;
            if(!val->StartUp(&err)){
                error_string_ = err;
                return false;
            }
        }
        is_started_ = true;
        return true;
    }

    void Shutdown()
    {
        qDebug() << "shutting down: " << GetPluginFilePath().data() << " ...";
        for(IPlugin *val: iplugin_objs_){
            val->Shutdown();
        }
        is_started_ = false;
    }

    virtual std::map<std::string, std::string> PluginInfo() const override
    {
        return plugin_info_dict_;
    }

    virtual std::string GetPluginFilePath() const override
    {
        return lib_.fileName().toLocal8Bit().data();
    }

    void AddRef()
    {
        ref_ ++;
    }

    void SubRef()
    {
        Q_ASSERT(ref_.load() > 0);
        ref_ --;
    }

    int CountRef()
    {
        return ref_.load();
    }

    std::string GetErrorString()
    {
        return error_string_;
    }

private:
    std::map<std::string, std::string> plugin_info_dict_;
    std::string error_string_;
    QLibrary lib_;
    std::vector<IPlugin *> iplugin_objs_;
    bool is_started_;

    FInitPlugin InitPlugin_;
    FUninitPlugin UninitPlugin_;
    FQueryObject QueryObjects_;
    FGetPluginInfo GetPluginInfo_;

    std::atomic_int32_t ref_;
};


struct PluginManager::Private
{
    QList<PluginSpec *> plugins;
    QList<IPluginSpec *> loading_list;
    QList<std::string> added_plugins;
    QList<std::string> retained_interfaces;
    QMap<std::string, std::list<void *>> queried_objs;
    QString lastError;
};


static PluginManager* g_pPluginManager = nullptr;

PluginManager *PluginManager::Instance()
{
    if(g_pPluginManager == nullptr){
        g_pPluginManager = new PluginManager;
    }
    return g_pPluginManager;
}

void PluginManager::Init()
{
    Q_ASSERT(private_ == nullptr);
    private_ = new Private;
}

void PluginManager::Uninit()
{
    if(private_->plugins.size() > 0){
        RemovePlugins();
    }

    Q_ASSERT(private_ != nullptr);
    delete private_;
    private_ = nullptr;

    delete g_pPluginManager;
    g_pPluginManager = nullptr;
}

void PluginManager::AddPlugin(const std::string &filePath)
{
    if(QFile::exists(filePath.c_str())){
        private_->added_plugins.append(filePath);
    }
    else{
        Q_ASSERT(false);
    }
}

void PluginManager::AddPlugins(const std::string &dirPath)
{
    std::list<IPluginSpec *> plugins;
    QDir dir(QString::fromStdString(dirPath));
    //TODO: define a unified suffix in both windows and mac os
    QStringList filter;
    filter.append("*.dll");
    filter.append("*.dylib");
    QFileInfoList files = dir.entryInfoList(filter);
    for(QFileInfo file: files){
        AddPlugin(file.absoluteFilePath().toStdString());
    }
}

void PluginManager::RemovePlugin(const std::string &filePath)
{
    Q_ASSERT(private_->added_plugins.contains(filePath));
    private_->added_plugins.removeAll(filePath);
}

void PluginManager::RemovePlugins()
{
    private_->added_plugins.clear();
}

std::list<IPluginSpec *> PluginManager::LoadAddedPlugins()
{
    QList<std::string> added_plugins = private_->added_plugins;
    for(std::string path: added_plugins){
        PluginSpec *spec = new PluginSpec(path);
        if(!spec->Load()){
            RemovePlugin(spec->GetPluginFilePath());
            Q_ASSERT(false);
            delete spec;
            continue;
        }
        private_->plugins.append(spec);
    }

    std::list<IPluginSpec *> plugins;
    QList<PluginSpec *> plugins_tmp = private_->plugins;
    for(PluginSpec *spec: plugins_tmp){
        if(spec->IsStarted()){
            continue;
        }
        private_->loading_list.append(spec);
        if(!spec->Startup()){
            Q_ASSERT(false);
            UnloadAndRemovePlugin(spec);
        }
        private_->loading_list.removeAll(spec);
    }

    return plugins;
}

std::list<IPluginSpec *> PluginManager::GetLoadedPlugins()
{
    std::list<IPluginSpec *> plugins;
    std::copy(private_->plugins.begin(), private_->plugins.end(), std::back_inserter(plugins));
    return plugins;
}

bool PluginManager::RetainObjects(const std::string &iid)
{
    qDebug() << "retain objects: " << iid.c_str();
    bool has_objs = false;
    private_->retained_interfaces.append(iid);
    for(PluginSpec *spec: private_->plugins){
        if(spec->IsSupportInterface(iid)){
            spec->AddRef();
            has_objs = true;
        }
    }
    return has_objs;
}

void PluginManager::ReleaseObjects(const std::string &iid)
{
    qDebug() << "release objects: " << iid.c_str();
    private_->retained_interfaces.removeAll(iid);
    for(PluginSpec *spec: private_->plugins){
        if(spec->IsSupportInterface(iid)){
            spec->SubRef();
        }
    }
}

std::list<void *> PluginManager::QueryObjectsInternal(const std::string &iid)
{
    if(!private_->queried_objs.contains(iid)){
        private_->queried_objs[iid] = std::list<void *>();
        for(PluginSpec *spec: private_->plugins){
            std::vector<void *> objs;
            spec->QueryObjects(iid, objs);
            for(void *obj: objs){
                private_->queried_objs[iid].push_back(obj);
            }
        }
    }

    return private_->queried_objs[iid];
}

void PluginManager::TraverseObjects(IPluginSpec *spec)
{
    for(std::string iid: private_->queried_objs.keys()){
        std::vector<void *> objs;
        static_cast<PluginSpec *>(spec)->QueryObjects(iid, objs);
        for(void *obj: objs){
            private_->queried_objs[iid].push_back(obj);
        }
    }
}

IPluginSpec * PluginManager::LoadPlugin(const std::string &filePath)
{
    AddPlugin(filePath);
    PluginSpec *spec = new PluginSpec(filePath);
    if(!spec->Load()){
        private_->lastError = spec->GetErrorString().c_str();
        RemovePlugin(spec->GetPluginFilePath());
        Q_ASSERT(false);
        delete spec;
        return nullptr;
    }
    private_->plugins.append(spec);
    if(!spec->Startup()){
        Q_ASSERT(false);
        UnloadAndRemovePlugin(spec);
        return nullptr;
    }
    TraverseObjects(spec);
    return spec;
}

bool PluginManager::UnloadAndRemovePlugin(IPluginSpec *pPluginSpec)
{
    PluginSpec *spec = static_cast<PluginSpec *>(pPluginSpec);
    if(spec->CountRef() > 0){
        private_->lastError = "this plugin is in used!";
        Q_ASSERT(false);
        return false;
    }

    private_->queried_objs.clear();
    spec->Shutdown();
    spec->Unload();
    RemovePlugin(spec->GetPluginFilePath());
    private_->plugins.removeAll(spec);
    delete spec;

    return true;
}

void PluginManager::UnloadAndRemovePlugins()
{
    while(private_->plugins.size() > 0){
        PluginSpec *spec = nullptr;
        for(PluginSpec *val: private_->plugins){
            if(val->CountRef() == 0){
                spec = val;
                break;
            }
        }
        if(spec != nullptr){
            UnloadAndRemovePlugin(spec);
        }
        else{
            Q_ASSERT(false);
            private_->lastError = "Circular dependency detected!";
            break;
        }
    }
}

std::string PluginManager::GetLastErrorString()
{
    return private_->lastError.toStdString();
}

PluginManager::PluginManager():
    private_(nullptr)
{

}

}// exsys
