TEMPLATE = app
CONFIG += debug qt warn_on

include(flatbase.pro)
include(flatgfx.pro)
TARGET=linegr
SOURCES+=linegr.cpp

DEFINES += WIN32 Q_OS_WIN32
INCLUDEPATH += ..

