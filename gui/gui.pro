# gui.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-gui
include(../config.pro)

HEADERS += guibutton.h guipage.h guiroot.h guigeom.h \
	timebutton.h guiexc.h overridecursor.h \
        videogui.h
SOURCES += guibutton.cpp guipage.cpp guiroot.cpp guigeom.cpp \
	timebutton.cpp guiexc.cpp overridecursor.cpp \
	videogui.cpp
HEADERS += abstractpage.h pagebuildgeom.h
SOURCES += abstractpage.cpp pagebuildgeom.cpp
HEADERS += guibuttongroup.h guiitem.h guitriangle.h
SOURCES += guibuttongroup.cpp guiitem.cpp guitriangle.cpp
HEADERS += guimenu.h guichecklist.h guitabbedpage.h
SOURCES += guimenu.cpp guichecklist.cpp guitabbedpage.cpp
HEADERS += guimenuitem.h guichecklistitem.h autoitems.h autobuttons.h
SOURCES += guimenuitem.cpp guichecklistitem.cpp autoitems.cpp autobuttons.cpp

LIBS += -L../base -lvscope-base -L../gfx -lvscope-gfx -L../xml -lvscope-xml
LIBS += -L../video -lvscope-video
