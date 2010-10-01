TEMPLATE = app
include(../config.pro)

TARGET=ffttest
SOURCES=ffttest.cpp
LIBS += -L.. -L../base -L../math -lvsdscope-math -lvsdscope-base
