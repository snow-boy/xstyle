TEMPLATE = lib
CONFIG += staticlib

!isEmpty(top_builddir){
    OUT_DIR = $$top_builddir
}
else{
    OUT_DIR = $$OUT_PWD
}

DESTDIR = $$OUT_DIR/lib

CONFIG += c++11
