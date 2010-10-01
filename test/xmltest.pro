TEMPLATE = app
include(../config.pro)

TARGET=xmltest
SOURCES=xmltest.cpp
LIBS += -L.. -L../base -L../gfx -L../xml -lvsdscope-gfx -lvsdscope-base -lvsdscope-xml
