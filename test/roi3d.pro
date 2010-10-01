TEMPLATE = app
include(../config.pro)

TARGET=roi3d
SOURCES+=roi3d.cpp
LIBS += -L.. -L../base -L../gfx -lvsdscope-gfx -lvsdscope-base
