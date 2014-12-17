# xml.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-xml
include(../config.pro)

HEADERS += enumerator.h   xmlfind.h   param.h   attribute.h   paramtree.h 
SOURCES += enumerator.cpp xmlfind.cpp param.cpp attribute.cpp paramtree.cpp
HEADERS += connections.h   script.h   settingsfile.h   aliases.h
SOURCES += connections.cpp script.cpp settingsfile.cpp aliases.cpp
HEADERS += definestimulus.h
SOURCES += definestimulus.cpp

LIBS += -L../base -lvscope-base
