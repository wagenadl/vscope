# QMake project file for vscope                  -*- mode: shell-script; -*-
#
# When adding source files, run tools/updatesources.sh to include them

TEMPLATE = app
unix: TARGET = ../build/vscope

include(vscope.pri)

INCLUDEPATH+=utils/mtpsd/include

unix: CONFIG += debug_and_release

CONFIG += warn_on resources

QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

DEPENDPATH +=  $$sourcedirs

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
  INCLUDEPATH += "C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\include"
  LIBS += -L"C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\lib\msvc"
  LIBS += -lNIDAQmx
  LIBS += -L"..\pvcam\i386"
  LIBS += -lpvcam32
  SOURCES -= daq/daqdummy.cpp
  SOURCES -= pvp/pvpDummy.cpp
}

linux {
#    SOURCES += daq/daqdummy.c
}

win32: RC_FILE = vscope.rc

unix: CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

win32 {
  DEFINES += WIN32
  DEFINES += vsdWIN32
}

cygwin-g++ {
  DEFINES += vsdWIN32
}

linux {
    DEFINES += vsdLINUX
}

DISTFILES += \
    ../pvcam/i386/pvcam32.Lib \
    ../tools/vscope-windeploy.pl
