TEMPLATE = app
include(../config.pro)

TARGET=base26
SOURCES+=base26.cpp
LIBS += -L.. -L../base -lvsdscope-base
