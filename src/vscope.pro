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

OBJECTS_DIR=../build/release
CONFIG(debug, debug|release) { OBJECTS_DIR=../build/debug }
MOC_DIR = $${OBJECTS_DIR}
RCC_DIR = $${OBJECTS_DIR}
UI_DIR = $${OBJECTS_DIR}

greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += c++11
} else {
    QMAKE_CXXFLAGS += -std=c++11
}


win32 {
  message($$QTDIR)
  ##LIBS += -L"c:\Qt\Qt5.7.0\Tools\mingw530_32\i686-w64-mingw32\lib"
  LIBS += -L"C:\Users\wagenaar\Documents\progs\NIDAQ Stuff\lib\msvc"
  LIBS += -lnidaqmx
  SOURCES -= daq/daqdummy.cpp
}


win32 {
#    RC_FILE = App/winicon.rc
#    LIBS += -lSecur32
}

for(sd, sourcedirs): include($${sd}/$${sd}.pri)

CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

win32 {
  DEFINES += WIN32  vsdWIN32
#  include win.pri
}
cygwin-g++ {
  DEFINES += vsdWIN32
}

linux-g++ {
  DEFINES += vsdLINUX
}
