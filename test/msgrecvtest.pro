TEMPLATE = app
include(../config.pro)

TARGET=msgrecvtest
SOURCES+=msgrecvtest.cpp
LIBS += -L.. -L../base -L../daq -lvsdscope-daq -lvsdscope-base -L../video -lvsdscope-video \
	-L../xml -lvsdscope-xml -L../gui -lvsdscope-gui -L../gfx -lvsdscope-gfx

win32 {
  LIBS += -lnicaiu
}
