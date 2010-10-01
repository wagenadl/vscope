TEMPLATE = app
include(../config.pro)

TARGET=linegr
SOURCES=linegr.cpp
LIBS += -L.. -L../base -L../gfx -lvsdscope-gfx -lvsdscope-base
