# video.pro, part of vscope
# (C) Daniel Wagenaar, 2009

TEMPLATE = lib
TARGET = vscope-video
include(../config.pro)

HEADERS += videoprog.h   videolight.h   videocommbase.h   videocomm.h   
SOURCES += videoprog.cpp videolight.cpp videocommbase.cpp

HEADERS += videocommplugins.h vcplugins.h
SOURCES += videocommplugins.cpp

HEADERS += vplugin.h   vp_flashsq.h   vp_flashcirc.h   vp_bars.h   vp_bar2.h
SOURCES += vplugin.cpp vp_flashsq.cpp vp_flashcirc.cpp vp_bars.cpp vp_bar2.cpp

LIBS +=  -L../base -lvscope-base -L../daq -lvscope-daq -L../xml -lvscope-xml

