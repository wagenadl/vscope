TEMPLATE = app
include(../config.pro)

TARGET=cohtest.bin
SOURCES=cohtest.cpp
LIBS += -L.. -L../base -L../math -lvscope-math -lvscope-base
