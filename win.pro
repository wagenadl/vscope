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
INCLUDEPATH += '/cygdrive/c/Program Files/Photometrics/PVCam32/sdk/inc'

LIBS += '/cygdrive/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/lib/msvc/NIDAQmx.lib'
LIBS += '/cygdrive/c/Program Files/Photometrics/PVCam32/sdk/lib/i386/Pvcam32.lib'
LIBS += -L$(QTDIR)/lib
LIBS += -lQtGui4 -lQtCore4 -lQtNetwork4 -lQtXml4

DEFINES+=WIN32 Q_OS_WIN32 vsdWIN32


RESOURCES += vsdscope-win.qrc

include(flat.pro)

RESOURCES += vsdscope-win.qrc


