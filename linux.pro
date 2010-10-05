#CONFIG += debug
CONFIG += qt ordered
CONFIG += warn_on

TEMPLATE = subdirs

DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .moc
OBJECTS_DIR = .obj

SUBDIRS = base math gfx xml pvp daq video gui acq toplevel
# SUBDIRS += test
DEFINES += vsdLINUX

