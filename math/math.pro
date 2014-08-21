# math.pro, part of vscope
# (C) Daniel Wagenaar, 2009

TEMPLATE = lib
TARGET = vscope-math
include(../config.pro)

HEADERS += cplx.h fft.h psdest.h taperid.h tapers.h taperbank.h
HEADERS += cohest.h cohestci.h cohdata.h floatimage.h
SOURCES += fft.cpp psdest.cpp taperid.cpp tapers.cpp taperbank.cpp
SOURCES += cohest.cpp cohestci.cpp cohdata.cpp floatimage.cpp

HEADERS += cvec.h
SOURCES += cvec.cpp

LIBS += -L../base -lvscope-base
