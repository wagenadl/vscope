# QMake project file for vscope                  -*- mode: shell-script; -*-
#
# When adding source files, run tools/updatesources.sh to include them

TEMPLATE = app
TARGET = ../build/vscope

include(vscope.pri)
SOURCSE -= 

INCLUDEPATH+=utils/mtpsd/include

CONFIG += debug_and_release warn_on resources

QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

DEPENDPATH +=  $$sourcedirs
INCLUDEPATH += $$sourcedirs

unix {
  OBJECTS_DIR=../build/release
  CONFIG(debug, debug|release) { OBJECTS_DIR=../build/debug }
}
MOC_DIR = $${OBJECTS_DIR}
RCC_DIR = $${OBJECTS_DIR}
UI_DIR = $${OBJECTS_DIR}

greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += c++11
} else {
    QMAKE_CXXFLAGS += -std=c++11
}


win32 {
message("win32")
  LIBS += -L"C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\lib\msvc"
  LIBS += -lNIDAQmx
  SOURCES -= daq/daqdummy.cpp
  SOURCES -= pvp/pvpDummy.cpp
}


win32 {
#    RC_FILE = App/winicon.rc
#    LIBS += -lSecur32
}

for(sd, sourcedirs): include($${sd}/$${sd}.pri)

CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

# message("HEADERS: $$HEADERS" )
# message("SOURCES: $$SOURCES" )
# message("RESOURCES: $$RESOURCES" )
# message("INCLUDEPATH: $$sourcedirs" )
# message("TARGET: $$TARGET")

tgt_ver.target = toplevel/version.xml
tgt_ver.commands = ../tools/updateversion.sh $$tgt_ver.target
tgt_ver.depends = alwaysrun
alwaysrun.commands =

QMAKE_EXTRA_TARGETS += tgt_ver alwaysrun

win32 {
  DEFINES += WIN32
# DEFINES += Q_OS_WIN32
  DEFINES += vsdWIN32
#  include win.pri
}

unix {
  DEFINES += vsdLINUX
}
