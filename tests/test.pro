# test.pro, part of vscope
# (C) Daniel Wagenaar, 2011
# This is the basic configuration for all tests

include(../config.pro)

INCLUDEPATH += ../..
LIBS += -L../..
DEPENDPATH += ../..

LIBS += -L../../base   -lvscope-base
LIBS += -L../../gfx    -lvscope-gfx
LIBS += -L../../xml    -lvscope-xml
LIBS += -L../../gui    -lvscope-gui
LIBS += -L../../daq    -lvscope-daq
LIBS += -L../../pvp    -lvscope-pvp
LIBS += -L../../acq    -lvscope-acq
LIBS += -L../../video  -lvscope-video
LIBS += -L../../math   -lvscope-math
