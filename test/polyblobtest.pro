TEMPLATE = app
include(../config.pro)

TARGET=polyblobtest
SOURCES+=polyblobtest.cpp
LIBS += -L..
LIBS += -lvsdscope-gfx -lvsdscope-base -lvsdscope-xml 
