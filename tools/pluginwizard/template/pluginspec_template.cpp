#include "%pluginl%.h"

BEGIN_DEF_PLUGIN_INFO()
    PLUGIN_INFO("author", "snow_boy")
//    PLUGIN_INFO("key2", "value2")
//    PLUGIN_INFO("key3", "value3")
END_DEF_PLUGIN_INFO()

BEGIN_INIT_PLUGIN_INTERFACE()
    BEGIN_CLASS_INSTANCE(%plugin%)
        INTERFACE_ENTRY(exsys::IPlugin, IPLUGIN_IID)
    END_CLASS_INSTANCE()
END_INIT_PLUGIN_INTERFACE()
