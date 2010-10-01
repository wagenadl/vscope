CONFIG = debug qt resources warn_on
QTDIR=/cygdrive/c/Qt/4.4.1
QMAKESPEC=cygwin-g++
TEMPLATE = app
QT += xml phonon

DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .moc
OBJECTS_DIR = .obj

INCLUDEPATH += /cygdrive/c/Qt/4.4.1/include
INCLUDEPATH += /cygdrive/c/Qt/4.4.1/include/QtCore
INCLUDEPATH += /cygdrive/c/Qt/4.4.1/include/QtGui
INCLUDEPATH += /cygdrive/c/Qt/4.4.1/include/QtXml
INCLUDEPATH += /cygdrive/c/Qt/4.4.1/include/QtNetwork
INCLUDEPATH += '/cygdrive/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include'
INCLUDEPATH += '/cygdrive/c/Program Files/Photometrics/PVCam/SDK/Headers'

LIBS += '/cygdrive/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/lib/msvc/NIDAQmx.lib'
LIBS += '/cygdrive/c/Program Files/Photometrics/PVCam/SDK/Libs/Pvcam32.lib'
LIBS += -L/cygdrive/c/Qt/4.4.1/lib
LIBS += -lQtGui4 -lQtCore4 -lQtNetwork4 -lQtXml4 -lphonon

DEFINES+=WIN32 Q_OS_WIN32 vsdWIN32
