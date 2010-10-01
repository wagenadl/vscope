TEMPLATE = app
include(../config.pro)

TARGET=typetest
SOURCES+=typetest.cpp
LIBS += -L.. -L../base -lvsdscope-base 
