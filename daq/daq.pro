# daq.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-daq
include(../config.pro)

HEADERS += analogin.h analogout.h daqbase.h \
	daqtask.h  digitalin.h digitalout.h \
	digitalstep.h
SOURCES +=  analogin.cpp analogout.cpp daqbase.cpp \
	daqtask.cpp  digitalin.cpp digitalout.cpp \
	digitalstep.cpp

LIBS += -L../base -lvscope-base 

win32 {
  LIBS += -lnicaiu
}

!win32 {
  SOURCES += daqdummy.cpp
}
