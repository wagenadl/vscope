TEMPLATE = app
include(../config.pro)

TARGET=virtkb
SOURCES+=virtkb.cpp foo.cpp
HEADERS+=foo.h
LIBS += -L.. -L../base -L../gfx -lvsdscope-gfx -lvsdscope-base
