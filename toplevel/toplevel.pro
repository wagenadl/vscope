# toplevel.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = app
TARGET = ../vscope.bin
include(../config.pro)

HEADERS += vscopegui.h gt_slots.h version.h
SOURCES += vscopegui.cpp main.cpp
SOURCES += gt_slots.cpp gt_openclose.cpp gt_selected.cpp 
SOURCES += gt_clicked.cpp gt_paramchanged.cpp
SOURCES += checkdaq.cpp checkcam.cpp checkmea.cpp checkvid.cpp
HEADERS += acquire.h
SOURCES += acquire.cpp
HEADERS += mgauto.h   vsdtraces.h   coherence.h   cohgraph.h
SOURCES += mgauto.cpp vsdtraces.cpp coherence.cpp cohgraph.cpp
HEADERS += savedsettings.h   exptlog.h
SOURCES += savedsettings.cpp exptlog.cpp
HEADERS += roisetguard.h
SOURCES += roisetguard.cpp
HEADERS += panelhistory.h
SOURCES += panelhistory.cpp
 
LIBS += -L../base -L../gfx -L../xml -L../gui
LIBS += -lvscope-base -lvscope-gfx -lvscope-xml -lvscope-gui
LIBS += -L../daq -L../pvp -lvscope-daq -lvscope-pvp
LIBS += -L../acq -lvscope-acq
LIBS += -L../video -lvscope-video
LIBS += -L../math -lvscope-math

RESOURCES += vscope.qrc
