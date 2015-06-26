# video.pro, part of vscope
# (C) Daniel Wagenaar, 2015

TEMPLATE = lib
TARGET = vscope-video
include(../config.pro)

HEADERS += vprojector.h
SOURCES += vprojector.cpp

LIBS +=  -L../base -lvscope-base -L../xml -lvscope-xml

