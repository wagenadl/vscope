# vsdscope/config.pro - Standard configuration variables for all subdirectories

CONFIG += qt debug warn_on resources
QT += xml
# QT += phonon

INCLUDEPATH += .
INCLUDEPATH += ..
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEPENDPATH += .
DEPENDPATH += ..
LIBS += -L..

win32 {
  DEFINES += WIN32
  DEFINES += Q_OS_WIN32
}
unix {
  QMAKESPEC=g++
  DEFINES+=vsdLINUX
}
