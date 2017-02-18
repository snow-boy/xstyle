#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <list>
#include <string>
#include "pluginmacros.h"

namespace exsys{

#define IPLUGIN_IID "DF35F4F9-FBE4-4BB7-AE4E-ECB76A1840E5"

class IPlugin
{
public:
    virtual ~IPlugin(){}

    virtual bool StartUp(std::string *errMsg) = 0;

    virtual void Shutdown() = 0;
};

}// exsys

#endif // IPLUGIN_H
