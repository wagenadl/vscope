# pvp.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-pvp
include(../config.pro)

HEADERS += pvpBase.h pvpAccess.h pvpEnumInt.h pvpException.h \
	pvpSystem.h pvpCamera.h \
	pvp_Class0.h pvp_Class2.h pvp_Class3.h \
	dwpvcam.h linuxpvcam.h linuxmaster.h
HEADERS += camera.h \
	ccdbinning.h ccdregion.h ccdtrigmode.h \
	ccdconfig.h campool.h

SOURCES += pvpAccess.cpp pvpException.cpp \
	pvpSystem.cpp pvpCamera.cpp \
	pvp_Class0.cpp pvp_Class2.cpp pvp_Class3.cpp
SOURCES += camera.cpp \
	ccdbinning.cpp ccdregion.cpp ccdtrigmode.cpp \
	ccdconfig.cpp campool.cpp

LIBS += -L../base -lvscope-base

!win32 {
  SOURCES += pvpDummy.cpp
}
