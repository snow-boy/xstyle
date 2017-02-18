TARGET = launcher

include($$PWD/../app.pri)

QT += widgets

SOURCES += \
    main.cpp

win32 {
    LIBS += $$LIBS_LINK_PATH/exsys.lib
}

mac {
    LIBS += $$LIBS_LINK_PATH/libexsys.dylib
}

CONFIG += c++11
