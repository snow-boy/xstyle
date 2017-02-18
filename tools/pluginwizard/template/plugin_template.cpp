#include "%pluginl%.h"
#include "exsys/pluginmanager.h"

%plugin%::%plugin%()
{
}

bool %plugin%::StartUp(std::string *errMsg)
{
    Q_UNUSED(errMsg)
    
    return true;
}

void %plugin%::Shutdown()
{
}

