# gfx.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-gfx
include(../config.pro)

HEADERS += ccdimage.h traceinfo.h linegraph.h lineedit.h textentry.h 
HEADERS += button.h ellipse.h multigraph.h
HEADERS += vsdgraph.h vsdallgraph.h virtkeyboard.h filelist.h
HEADERS += scrollbuttonbar.h filechooser.h filedialog.h hscrollbar.h
HEADERS += filedlgkey.h blackout.h visiblob.h
HEADERS += exptdirlist.h trialfilelist.h exptfiledialog.h
HEADERS += buttongrouper.h roiimage.h radialgraph.h
HEADERS += colors.h roiimages.h ccdscroll.h

SOURCES += ccdimage.cpp traceinfo.cpp linegraph.cpp lineedit.cpp textentry.cpp
SOURCES += button.cpp ellipse.cpp multigraph.cpp
SOURCES += vsdgraph.cpp  vsdallgraph.cpp virtkeyboard.cpp filelist.cpp
SOURCES += scrollbuttonbar.cpp filechooser.cpp filedialog.cpp hscrollbar.cpp
SOURCES += filedlgkey.cpp blackout.cpp visiblob.cpp
SOURCES += exptdirlist.cpp trialfilelist.cpp exptfiledialog.cpp
SOURCES += buttongrouper.cpp roiimage.cpp radialgraph.cpp
SOURCES += colors.cpp roiimages.cpp ccdscroll.cpp

LIBS += -L../base -lvscope-base
