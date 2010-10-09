# xml.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-xml
include(../config.pro)

HEADERS += enumerator.h xmlfind.h param.h attribute.h paramtree.h \
	 connections.h script.h
SOURCES += enumerator.cpp xmlfind.cpp param.cpp attribute.cpp paramtree.cpp \
	connections.cpp script.cpp

LIBS += -L../base -lvscope-base
