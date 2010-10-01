TEMPLATE = app
include(../config.pro)

TARGET=filedialog
SOURCES+=filedialog.cpp 
LIBS += -L.. -L../base -L../gfx -lvsdscope-gfx -lvsdscope-base
