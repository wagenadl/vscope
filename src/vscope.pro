# QMake project file for vscope                  -*- mode: shell-script; -*-
#
# When adding source files, run tools/updatesources.sh to include them

TEMPLATE = app
unix: TARGET = vscope

include(vscope.pri)

INCLUDEPATH+=utils/mtpsd/include

unix: CONFIG += debug_and_release

CONFIG += warn_on resources

QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

DEPENDPATH +=  $$sourcedirs

greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += c++11
} else {
    QMAKE_CXXFLAGS += -std=c++11
}


win32 {
  message(win32: pwd=$$PWD)
  INCLUDEPATH += "C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\include"
  LIBS += -L$$PWD/../nidaq
  LIBS += -lNIDAQmx
  LIBS += -L$$PWD/../pvcam/i386
  LIBS += -lpvcam32
  #LIBS += "..\pvcam\i386\pvcam32.lib"
  SOURCES -= daq/daqdummy.cpp
  SOURCES -= pvp/pvpDummy.cpp
}

win32: RC_FILE = vscope.rc

CONFIG(debug, debug|release) { TARGET=$${TARGET}_debug }

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

gitREVNO = $$system(git rev-list --count --first-parent HEAD)
gitDATE = $$system(git show -s --format=%cd --date=format:\"%b.%d.%Y..%H:%M:%S\" HEAD)
# For some reason, the %Z or %s
gitVERSION = $$system(git describe --long)
gitYEAR = $$system(git show -s --format=%cd --date=format:%Y HEAD)

DEFINES += gitREVNO=\\\"$$gitREVNO\\\"
DEFINES += gitDATE=\\\"$$gitDATE\\\"
DEFINES += gitVERSION=\\\"$$gitVERSION\\\"
DEFINES += gitYEAR=\\\"$$gitYEAR\\\"

message(revno: $$gitREVNO)
message(date: $$gitDATE)
message(version: $$gitVERSION)
message(year: $$gitYEAR)
