#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <string>
#include <list>
#include <deque>
#include <map>
#include <memory>
#include "exsys_global.h"
#include "iplugin.h"
#include "pluginmacros.h"

namespace exsys{

class IPluginSpec
{
public:
    virtual ~IPluginSpec(){}

    virtual std::map<std::string, std::string> PluginInfo() const = 0;

    virtual std::string GetPluginFilePath() const = 0;
};


class EXTENSIONSYSTEM_DECL PluginManager
{
public:
    static PluginManager* Instance();

    void Init();

    void Uninit();

    void AddPlugin(const std::string& filePath);

    void AddPlugins(const std::string& dirPath);

    void RemovePlugin(const std::string& filePath);

    void RemovePlugins();

    std::list<IPluginSpec *>  LoadAddedPlugins();

    IPluginSpec * LoadPlugin(const std::string& filePath);

    bool UnloadAndRemovePlugin(IPluginSpec *pPluginSpec);

    void UnloadAndRemovePlugins();

    std::string GetLastErrorString();

    std::list<IPluginSpec*> GetLoadedPlugins();

    template<typename T>
    T* QueryObject(const std::string &iid)
    {
        T *obj = nullptr;
        std::list<T*> objs = QueryObjects<T>(iid);
        if(objs.size() > 0){
            obj = *objs.begin();
        }
        return obj;
    }

    template<typename T>
    std::list<T*> QueryObjects(const std::string &iid)
    {
        std::list<void *> void_objs = QueryObjectsInternal(iid);
        std::list<T*> objs;
        for(void *val: void_objs){
            objs.push_back(static_cast<T*>(val));
        }
        return objs;
    }

    bool RetainObjects(const std::string &iid);

    void ReleaseObjects(const std::string &iid);

private:
    std::list<void *> QueryObjectsInternal(const std::string &iid);

    void TraverseObjects(IPluginSpec *spec);

    PluginManager();
    struct Private;
    Private* private_;
};

}// exsys

#define PlnMgr (*exsys::PluginManager::Instance())

#endif // PLUGINMANAGER_H
