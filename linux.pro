#CONFIG += debug
CONFIG += qt
CONFIG += warn_on

TEMPLATE = subdirs

DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .moc
OBJECTS_DIR = .obj

SUBDIRS = base math gfx xml gui pvp daq acq video toplevel
# SUBDIRS += test
DEFINES += vsdLINUX

