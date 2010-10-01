TEMPLATE = app
include(../config.pro)

TARGET=guitest
HEADERS+=gt_slots.h
SOURCES+=guitest.cpp gt_slots.cpp
LIBS += -L..
LIBS += -lvsdscope-gfx -lvsdscope-base -lvsdscope-xml -lvsdscope-gui
#LIBS += -lvsdscope-daq -lvsdscope-pvp -lvsdscope-acq

HEADERS+=../toplevel/vsdscopegui.h
SOURCES+=../toplevel/vsdscopegui.cpp
