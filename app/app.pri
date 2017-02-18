TEMPLATE = app

!isEmpty(top_srcdir) {
    INCLUDEPATH += $$top_srcdir/plugins
    INCLUDEPATH += $$top_srcdir/libs
}

!isEmpty(top_builddir){
    OUT_DIR = $$top_builddir
}
else{
    OUT_DIR = $$OUT_PWD
}

DESTDIR = $$OUT_DIR/bin

win32 {
    LIBS_LINK_PATH = $$OUT_DIR/lib
    PLUGIN_LINK_PATH = $$OUT_DIR/lib
    LIBS += $$LIBS_LINK_PATH/exsys.lib
}

mac {
    QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../,-rpath,@executable_path/../
    LIBS_LINK_PATH = $$DESTDIR/$${TARGET}.app/contents/Frameworks
    PLUGIN_LINK_PATH = $$DESTDIR/$${TARGET}.app/contents/plugins
}
