TEMPLATE = app
include(../config.pro)

TARGET=buttontest
SOURCES=buttontest.cpp
LIBS += -L.. -lvsdscope-gfx -lvsdscope-base
