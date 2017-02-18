#include "mainwindow.h"
#include "exsys/pluginmanager.h"

mainwindow::mainwindow()
{
}

bool mainwindow::StartUp(std::string *errMsg)
{
    Q_UNUSED(errMsg)
    
    return true;
}

void mainwindow::Shutdown()
{
}

