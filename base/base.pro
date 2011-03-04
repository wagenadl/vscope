# base.pro, part of vscope
# (C) Daniel Wagenaar, 2008

TEMPLATE = lib
TARGET = vscope-base
include(../config.pro)

HEADERS += base26.h \
           dbg.h \
	   exception.h \
           memalloc.h \
	   minmax.h \
           numbers.h \
           range.h \
	   xyrra.h \
           types.h \
 	   solvep.h \
 	   cplx.h \
	ccddata.h roidata.h roi3data.h \
	polyblob.h blobroi.h \
	roiset.h roidata3set.h \
	analogdata.h digitaldata.h \
	xml.h unitqty.h xyabc.h \
        ccdtiming.h \
        roicoords.h transform.h campair.h \
        ptrguard.h object.h
	   
SOURCES += base26.cpp \
           dbg.cpp \
	   exception.cpp \
           numbers.cpp \
           xyrra.cpp \
	   types.cpp \
           solvep.cpp \
	   cplx.cpp \
	ccddata.cpp roidata.cpp roi3data.cpp \
	polyblob.cpp blobroi.cpp \
	roiset.cpp roidata3set.cpp \
	analogdata.cpp digitaldata.cpp \
	xml.cpp unitqty.cpp xyabc.cpp \
        ccdtiming.cpp memalloc.cpp \
        roicoords.cpp transform.cpp object.cpp



