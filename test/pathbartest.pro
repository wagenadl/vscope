TEMPLATE = app
include(../config.pro)

TARGET=pathbartest
SOURCES+=pathbartest.cpp
LIBS += -L.. -L../base -L../gfx -lvsdscope-gfx -lvsdscope-base
