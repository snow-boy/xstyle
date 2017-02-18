TEMPLATE = subdirs

SUBDIRS = \
    libs\
    launcher\
    mainwindow

launcher.subdir = app/launcher
launcher.depends = libs

mainwindow.subdir = modules/mainwindow
