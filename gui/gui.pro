# gui.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-gui
include(../config.pro)

HEADERS += guibutton.h   guipage.h   guiroot.h   guigeom.h
SOURCES += guibutton.cpp guipage.cpp guiroot.cpp guigeom.cpp

HEADERS += timebutton.h   guiexc.h   overridecursor.h  
SOURCES += timebutton.cpp guiexc.cpp overridecursor.cpp

HEADERS += videogui.h  
SOURCES += videogui.cpp
HEADERS += abstractpage.h   pagebuildgeom.h  
SOURCES += abstractpage.cpp pagebuildgeom.cpp

HEADERS += guiitem.h   guitriangle.h  
SOURCES += guiitem.cpp guitriangle.cpp

HEADERS += guimenu.h   guichecklist.h   guitabbedpage.h  
SOURCES += guimenu.cpp guichecklist.cpp guitabbedpage.cpp

HEADERS += guimenuitem.h   guichecklistitem.h   autoitems.h   autobuttons.h  
SOURCES += guimenuitem.cpp guichecklistitem.cpp autoitems.cpp autobuttons.cpp

HEADERS += guiradiogroup.h   guitabctrl.h  
SOURCES += guiradiogroup.cpp guitabctrl.cpp

LIBS += -L../base -lvscope-base -L../gfx -lvscope-gfx -L../xml -lvscope-xml
LIBS += -L../video -lvscope-video
