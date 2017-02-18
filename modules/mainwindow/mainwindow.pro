TARGET = mainwindow
include($$PWD/../plugin.pri)

QT += widgets core

SOURCES += \
    mainwindow.cpp \
    mainwindowspec.cpp

HEADERS += \
    mainwindow.h 


win32 {
    LIBS += $$DYLIB_LINK_PATH/exsys.lib

    CONFIG(debug, debug|release) {
        contains(QMAKE_TARGET.arch, x86) {
        }
        contains(QMAKE_TARGET.arch, x86_64) {
        }
    }
    CONFIG(release, debug|release) {
        contains(QMAKE_TARGET.arch, x86) {
        }
        contains(QMAKE_TARGET.arch, x86_64) {
        }
    }
}

mac {
    LIBS += $$DYLIB_LINK_PATH/extensionsystem.dylib

    CONFIG(debug, debug|release) {
    }
    CONFIG(release, debug|release) {
    }
}
