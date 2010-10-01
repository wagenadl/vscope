TEMPLATE = app
CONFIG += debug qt warn_on
QT += xml

include(flatbase.pro)
include(flatxml.pro)
INCLUDEPATH += ..
DEFINES += WIN32 Q_OS_WIN32
TARGET=xmltest
SOURCES+=xmltest.cpp
