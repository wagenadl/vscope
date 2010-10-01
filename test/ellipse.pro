TEMPLATE = app
include(../config.pro)

TARGET=ellipse
SOURCES+=ellipse.cpp
LIBS += -L..
LIBS += -lvsdscope-gfx -lvsdscope-base -lvsdscope-xml -lvsdscope-gui
