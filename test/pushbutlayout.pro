TEMPLATE = app
include(../config.pro)

TARGET=pushbutlayout
SOURCES+=pushbutlayout.cpp myscrollarea.cpp
HEADERS+=myscrollarea.h
LIBS += -L.. -L../base -L../gfx -lvsdscope-gfx -lvsdscope-base
