QTDIR=/cygdrive/c/Qt/4.4.3
QMAKESPEC=cygwin-g++

INCLUDEPATH += $(QTDIR)/include
INCLUDEPATH += $(QTDIR)/include/QtCore
INCLUDEPATH += $(QTDIR)/include/QtGui
INCLUDEPATH += $(QTDIR)/include/QtXml
INCLUDEPATH += $(QTDIR)/include/QtNetwork
INCLUDEPATH += '/cygdrive/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include'

exists('c:/Program Files/Photometrics/PVCam/sdk/headers') {
  INCLUDEPATH += '/cygdrive/c/Program Files/Photometrics/PVCam/sdk/headers'
} else {
  INCLUDEPATH += '/cygdrive/c/Program Files/Photometrics/PVCam32/sdk/inc'
}

LIBS += '/cygdrive/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/lib/msvc/NIDAQmx.lib'

exists('c:/Program Files/Photometrics/PVCam/sdk/libs/Pvcam32.lib') {
  LIBS += '/cygdrive/c/Program Files/Photometrics/PVCam/sdk/libs/Pvcam32.lib'
} else {
  LIBS += '/cygdrive/c/Program Files/Photometrics/PVCam32/sdk/lib/i386/Pvcam32.lib'
}

LIBS += -L$(QTDIR)/lib
LIBS += -lQtGui4 -lQtCore4 -lQtNetwork4 -lQtXml4

RESOURCES += vsdscope-win.qrc

QMAKE_EXTRA_TARGETS += version alwaysrun
PRE_TARGETDEPS += toplevel/version.xml
version.target = toplevel/version.xml
version.commands = utils/getversion $$version.target .
version.depends = alwaysrun
alwaysrun.commands = 
