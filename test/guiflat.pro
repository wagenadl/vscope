TEMPLATE = app

CONFIG += debug qt warn_on
QT += xml

include(flatbase.pro)
include(flatxml.pro)
include(flatgfx.pro)
include(flatgui.pro)

TARGET=guitest
HEADERS+=gt_slots.h
SOURCES+=guitest.cpp gt_slots.cpp

SOURCES+=../toplevel/vsdscopegui.cpp
HEADERS+=../toplevel/vsdscopegui.h

INCLUDEPATH += ..
DEFINES += WIN32 Q_OS_WIN32
