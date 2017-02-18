#ifndef %plugindef%_H
#define %plugindef%_H
#include "exsys/iplugin.h"

class %plugin%: public exsys::IPlugin
{
public:
    %plugin%();

    virtual bool StartUp(std::string *errMsg) override;

    virtual void Shutdown() override;
};

#endif // %plugindef%_H
