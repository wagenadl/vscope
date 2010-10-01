CONFIG = release
CONFIG += qt
CONFIG += warn_on thread windows
QT += xml
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
SOURCES += daqtest.cpp
INCLUDEPATH += ..
LIBS += -L..
LIBS += -lnicaiu

win32 {
  DEFINES += WIN32 Q_OS_WIN32
}
