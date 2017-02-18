#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "exsys/iplugin.h"

class mainwindow: public exsys::IPlugin
{
public:
    mainwindow();

    virtual bool StartUp(std::string *errMsg) override;

    virtual void Shutdown() override;
};

#endif // MAINWINDOW_H
