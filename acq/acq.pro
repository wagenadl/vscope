# acq.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-acq
include(../config.pro)

HEADERS += liveephys.h   focus.h   ephysout.h   ccdacq.h
SOURCES += liveephys.cpp focus.cpp ephysout.cpp ccdacq.cpp
HEADERS += ephysacq.h   trial.h   contacq.h   vco.h    trialsig.h
SOURCES += ephysacq.cpp trial.cpp contacq.cpp vco.cpp  trialsig.cpp
HEADERS += trialdata.h   datatrove.h
SOURCES += trialdata.cpp datatrove.cpp

LIBS += -L../base -L../gfx -L../xml -L../gui
LIBS += -lvscope-base -lvscope-gfx -lvscope-xml -lvscope-gui
LIBS += -L../daq -L../pvp -lvscope-daq -lvscope-pvp
