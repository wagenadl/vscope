# gui.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-gui
include(../config.pro)

HEADERS += xmlbutton.h xmlpage.h xmlgui.h guigeom.h \
	timebutton.h guiexc.h overridecursor.h \
        videogui.h
SOURCES += xmlbutton.cpp xmlpage.cpp xmlgui.cpp guigeom.cpp \
	timebutton.cpp guiexc.cpp overridecursor.cpp \
	videogui.cpp
HEADERS += abstractpage.h pagebuildgeom.h autoiteminfo.h
SOURCES += abstractpage.cpp pagebuildgeom.cpp autoiteminfo.cpp
HEADERS += guibuttongroup.h
SOURCES += guibuttongroup.cpp

LIBS += -L../base -lvscope-base -L../gfx -lvscope-gfx -L../xml -lvscope-xml
LIBS += -L../video -lvscope-video
