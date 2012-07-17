CONFIG = debug qt resources warn_on
QTDIR=/cygdrive/c/Qt/4.4.3
#QTDIR=/usr/lib/qt4
QMAKESPEC=cygwin-g++
TEMPLATE = app
TARGET = vscope
# CONFIG += warn_on thread windows
QT += xml

DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .moc
OBJECTS_DIR = .obj

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

DEFINES+=WIN32 Q_OS_WIN32 vsdWIN32

include(flat.pro)

RESOURCES += vsdscope-win.qrc

QMAKE_EXTRA_TARGETS += version alwaysrun
PRE_TARGETDEPS += toplevel/version.xml
version.target = toplevel/version.xml
version.commands = utils/getversion $$version.target .
version.depends = alwaysrun
alwaysrun.commands = 
