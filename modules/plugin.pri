TEMPLATE = lib

unix {
    target.path = /usr/lib
    INSTALLS += target
}

!isEmpty(top_builddir) {
    OUT_DIR = $$top_builddir
}
else {
    OUT_DIR = $$OUT_PWD
}

EXTERNAL_PATH = $$top_srcdir/Externals
TOOLS_PATH = $$top_srcdir/Tools

!isEmpty(top_srcdir) {
    INCLUDEPATH += $$top_srcdir/plugins
    INCLUDEPATH += $$top_srcdir/libs
}

win32 {
    DESTDIR = $$OUT_DIR/lib
    DLLDESTDIR = $$OUT_DIR/bin/plugins
    SLIB_LINK_PATH = $$DESTDIR
    DYLIB_LINK_PATH = $$DESTDIR
    DYLIB_LOAD_PATH = $$OUT_DIR/bin
}

mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/PlugIns/
    APP_BUNDLE = $$files($$OUT_DIR/bin/*.app)
    !exists($$APP_BUNDLE){
        error(can not found app bundle)
    }
    DESTDIR = $$APP_BUNDLE/Contents/PlugIns
    SLIB_LINK_PATH = $$OUT_DIR/lib
    DYLIB_LINK_PATH = $$APP_BUNDLE/Contents/Frameworks
    DYLIB_LOAD_PATH = $$LIB_LINK_PATH
}

CONFIG += plugin plugin_with_soname c++11
