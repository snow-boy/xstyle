#include <QApplication>
#include <QDir>
#include "exsys/pluginmanager.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir dir(a.applicationDirPath());
#ifdef Q_OS_WIN
    dir.cd("plugins");
#elif defined(Q_OS_MAC)
    dir.cd("../PlugIns");
#endif

    PlnMgr.Init();
    PlnMgr.AddPlugins(dir.absolutePath().toStdString());
    PlnMgr.LoadAddedPlugins();
    QObject::connect(&a, &QApplication::aboutToQuit, [](){
        PlnMgr.UnloadAndRemovePlugins();
        PlnMgr.Uninit();
    });

    return a.exec();
}

