#
# MTPSD Makefile
# C. Antonio Sanchez
# 

MTPSD_VERSION=1.0

#Checks for operating system, sets compiler
OS_NAME=$(shell uname -s | tr A-Z a-z)
OS_CHECK=$(findstring w32, "$(OS_NAME)")
PROCESSOR=$(shell uname -m)

#sets C++ compiler
#NOTE: If compiling octave extensions, make sure to use compatible compiler
#      (I had to switch to mingw32)
ifneq "x$(OS_CHECK)" "x"

	CC=mingw32-g++-4.4.0-dw2.exe
	MINGW_PATH=/d/local/octave/mingw32
	LIB_PATH=/d/local/octave/lib

	PATH:=$(MINGW_PATH)/bin:$(PATH)
	LDFLAGS:=$(LDFLAGS) -L$(LIB_PATH)
else
	CC=g++
endif
OCT_BASE=`octave-config -p INCLUDEDIR`
OCT_VER=`octave-config -p VERSION`
CFLAGS=-c -g -O3 -W -Wall -I$(SRC)/include -I$(SRC)/src/dpss -I$(SRC)/src/mtpsd -I$(SRC)/src/common -I$(OCT_BASE) -I$(OCT_BASE)/octave-$(OCT_VER) -I$(OCT_BASE)/octave-$(OCT_VER)/octave -fPIC
LDFLAGS:= -llapack -lfftw3 $(LDFLAGS)

OCT_CC=mkoctfile
OCT_LDFLAGS=

TARGET_ARCH=$(shell $(CC) -v 2>&1 | grep Target | tr ' ' '\n' | grep -v Target | tr '-' '_')

COMMON_HRDS= $(SRC)/include/template_math.h \
	     $(SRC)/include/simple_error.h

COMMON_SRCS= $(SRC)/src/common/simple_error.cpp

COMMON_OBJS= $(patsubst $(SRC)/src/%.cpp,build/%.o,$(COMMON_SRCS))

DPSS_HRDS=   $(SRC)/include/dpss.h

DPSS_SRCS=     $(SRC)/src/dpss/dpss.cpp \
               $(SRC)/src/dpss/dpss_fftw.cpp

DPSS_OCT_SRCS= $(SRC)/src/dpss/dpss_octave.cpp
DPSS_CMD_SRCS= $(SRC)/src/dpss/dpss_cmd.cpp
DPSS_BIN_SRCS= $(SRC)/src/dpss/dpss_bin.cpp

DPSS_OBJS=   $(patsubst $(SRC)/src/%.cpp,build/%.o,$(DPSS_SRCS))
DPSS_OCT_OBJS= build/dpss/dpss_octave.o
DPSS_CMD_OBJS= build/dpss/dpss_cmd.o
DPSS_BIN_OBJS= build/dpss/dpss_bin.o

DPSS_OCT=   bin/dpss.oct
DPSS_LIB=   lib/libdpss.a
DPSS_CMD=   bin/dpss
DPSS_BIN=   bin/mkdpss

MTPSD_HDRS=  $(SRC)/include/mtpsd.h \
	     $(SRC)/include/applied_stats.h

MTPSD_SRCS=  $(SRC)/src/mtpsd/mtpsd.cpp \
             $(SRC)/src/common/applied_stats.cpp
MTPSD_OCT_SRCS= $(SRC)/src/mtpsd/mtpsd_octave.cpp

MTPSD_OBJS=  $(patsubst $(SRC)/src/%.cpp,build/%.o,$(MTPSD_SRCS))
MTPSD_OCT_OBJS= build/mtpsd/mtpsd_octave.o

MTPSD_OCT=   bin/mtpsd.oct
MTPSD_LIB=   lib/libmtpsd.a

.PHONY : clean install cleanall all oct lib dpss mtpsd dpss_lib mtpsd_lib dpss_oct mtpsd_oct

all:  bin/mkdpss  mtpsd_lib dpss_oct mtpsd_oct dpss_lib dpss_cmd dpss_bin

dpss: dpss_oct dpss_lib dpss_cmd

mtpsd: mtpsd_oct mtpsd_lib

oct: mtpsd_oct dpss_oct

nooct: dpss_lib dpss_cmd mtpsd_lib

lib: mtpsd_lib dpss_lib

cmd: dpss_cmd

dpss_oct:$(DPSS_OCT)

mtpsd_oct:$(MTPSD_OCT)

dpss_lib:$(DPSS_LIB)

mtpsd_lib:$(MTPSD_LIB)

dpss_cmd:$(DPSS_CMD)
dpss_bin:$(DPSS_BIN)

$(DPSS_CMD):$(COMMON_OBJS) $(DPSS_OBJS) $(DPSS_CMD_OBJS) $(DPSS_LIB)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	$(CC) $(DPSS_CMD_OBJS) -Llib -ldpss $(LDFLAGS) -o $@

$(DPSS_BIN):$(COMMON_OBJS) $(DPSS_OBJS) $(DPSS_BIN_OBJS) $(DPSS_LIB)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	$(CC) $(DPSS_BIN_OBJS) -Llib -ldpss $(LDFLAGS) -o $@
	strip $@

$(DPSS_OCT):$(COMMON_OBJS) $(DPSS_OBJS) $(DPSS_OCT_OBJS)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	$(OCT_CC) $(COMMON_OBJS) $(DPSS_OBJS) $(DPSS_OCT_OBJS) -o $@ $(OCT_LDFLAGS)
	strip $@

$(MTPSD_OCT):$(COMMON_OBJS) $(DPSS_OBJS) $(MTPSD_OBJS) $(MTPSD_OCT_OBJS)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	$(OCT_CC) $(COMMON_OBJS) $(DPSS_OBJS) $(MTPSD_OBJS) $(MTPSD_OCT_OBJS) -o $@ $(OCT_LDFLAGS)
	strip $@

$(MTPSD_LIB):$(COMMON_OBJS) $(DPSS_OBJS) $(MTPSD_OBJS)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	ar rcs $@ $(COMMON_OBJS) $(DPSS_OBJS) $(MTPSD_OBJS)
	ranlib $@

$(DPSS_LIB):$(COMMON_OBJS) $(DPSS_OBJS)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	ar rcs $@ $(COMMON_OBJS) $(DPSS_OBJS)
	ranlib $@

build/common/%.o:$(SRC)/src/common/%.cpp $(COMMON_HDRS)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	$(CC) $(CFLAGS) $< -o $@

build/dpss/%.o:$(SRC)/src/dpss/%.cpp $(COMMON_HDRS) $(DPSS_HDRS)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	$(CC) $(CFLAGS) $< -o $@

build/mtpsd/%.o:$(SRC)/src/mtpsd/%.cpp $(COMMON_HDRS) $(MTPSD_HDRS)
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)		#makes the build path if required
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf ${COMMON_OBJS} $(DPSS_OBJS) $(DPSS_OCT_OBJS) $(DPSS_CMD_OBJS) $(DPSS_BIN_OBJS) $(MTPSD_OBJS) $(MTPSD_OCT_OBJS)
	strip $(DPSS_CMD) $(DPSS_BIN) $(DPSS_OCT)
	rm -rf *~ */*~ */*/*~

cleanall: clean
	rm -rf ${DPSS_LIB} $(MTPSD_LIB) $(DPSS_OCT) $(MTPSD_OCT) $(DPSS_CMD)

