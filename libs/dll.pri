TEMPLATE = lib

!isEmpty(top_builddir){
    OUT_DIR = $$top_builddir
}
else{
    OUT_DIR = $$OUT_PWD
}

win32 {
    DESTDIR = $$OUT_DIR/lib
    DLLDESTDIR = $$OUT_DIR/bin
}

mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/Frameworks/
    APP_BUNDLE = $$files($$OUT_DIR/bin/*.app)
    !exists($$APP_BUNDLE){
        error(can not found app bundle)
    }
    DESTDIR = $$APP_BUNDLE/Contents/Frameworks
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
