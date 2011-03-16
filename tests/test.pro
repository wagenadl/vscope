# test.pro, part of vscope
# (C) Daniel Wagenaar, 2011
# This is the basic configuration for all tests

CONFIG = debug qt resources warn_on
QT += xml
MOC_DIR = .moc
OBJECTS_DIR = .obj

INCLUDEPATH += ../..
LIBS += -L../..
DEPENDPATH += ../..

unix {
LIBS += -L../../base   -lvscope-base
LIBS += -L../../gfx    -lvscope-gfx
LIBS += -L../../xml    -lvscope-xml
LIBS += -L../../gui    -lvscope-gui
LIBS += -L../../daq    -lvscope-daq
LIBS += -L../../pvp    -lvscope-pvp
LIBS += -L../../acq    -lvscope-acq
LIBS += -L../../video  -lvscope-video
LIBS += -L../../math   -lvscope-math
}

INCLUDEPATH += '/cygdrive/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include'
INCLUDEPATH += '/cygdrive/c/Program Files/Photometrics/PVCam/SDK/Headers'

LIBS += '/cygdrive/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/lib/msvc/NIDAQmx.lib'
LIBS += '/cygdrive/c/Program Files/Photometrics/PVCam/SDK/Libs/Pvcam32.lib'

win32 {
  include(../flattest.pro)
}
