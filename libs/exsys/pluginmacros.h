#ifndef PLUGINMACROS_H
#define PLUGINMACROS_H
#include "exsys_global.h"
#include <list>
#include <map>
#include <functional>

// usage:
//BEGIN_DEF_PLUGIN_INFO()
//    PLUGIN_INFO("key1", "value1")
//    PLUGIN_INFO("key2", "value2")
//END_DEF_PLUGIN_INFO()

//BEGIN_INIT_PLUGIN_INTERFACE()
//    BEGIN_CLASS_INSTANCE(className)
//        INTERFACE_ENTRY(Interface1, "iid1")
//        INTERFACE_ENTRY(Interface2, "iid2")
//    END_CLASS_INSTANCE()
//    BEGIN_CLASS_INSTANCE(className2)
//        INTERFACE_ENTRY(Interface21, "iid21")
//        INTERFACE_ENTRY(Interface22, "iid22")
//    END_CLASS_INSTANCE()
//END_INIT_PLUGIN_INTERFACE()


#define BEGIN_DEF_PLUGIN_INFO() \
    static const char s_plugin_info[] = "__noitom_pugin_spec_begin_tag__{"\
    "\"architecture\":\"1.0.0.0\""

#define PLUGIN_INFO(key, value)\
    ",\""\
    key\
    "\":\"" \
    value \
    "\""

#define END_DEF_PLUGIN_INFO() \
    "}";\
    extern "C" DECL_EXPORT const char * GetPluginInfo() \
    {\
        return s_plugin_info+31;\
    }


#define BEGIN_INIT_PLUGIN_INTERFACE()\
    static std::list<std::function<void (void)>> *s_clean_actions = nullptr; \
    static std::map<std::string, std::list<void *>> *s_objects = nullptr; \
    extern "C" DECL_EXPORT void InitializePlugin() \
    { \
        s_clean_actions = new std::list<std::function<void (void)>>(); \
        s_objects = new std::map<std::string, std::list<void *>>();

#define BEGIN_CLASS_INSTANCE(classname) \
    { \
        classname * inst = new classname();\
        s_clean_actions->push_back([inst](){delete inst;});

#define INTERFACE_ENTRY(interface, iid) \
    (*s_objects)[iid].push_back(static_cast<interface *>(inst));

#define END_CLASS_INSTANCE() \
    }

#define END_INIT_PLUGIN_INTERFACE() \
    }\
    extern "C" DECL_EXPORT void UninitializePlugin() \
    { \
        if(s_clean_actions != nullptr){\
            for(auto action: *s_clean_actions){ \
                action(); \
            } \
            delete s_clean_actions; \
            s_clean_actions = nullptr; \
        }\
        if(s_objects != nullptr){\
            delete s_objects; \
            s_objects = nullptr; \
        }\
    } \
    extern "C" DECL_EXPORT int QueryPluginObjects(const char * iid, void ** objs)\
    {\
        if(s_objects == nullptr){\
            return 0;\
        }\
        int inst_count = 0;\
        if(s_objects->count(iid) > 0){\
            inst_count = static_cast<int>((*s_objects)[iid].size());\
        } \
        if(objs != nullptr){\
            int i = 0;\
            for(auto val: (*s_objects)[iid]){\
                objs[i] = val;\
                ++i;\
            }\
        }\
        return inst_count;\
    }


#endif // PLUGINMACROS_H
