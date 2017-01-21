DEFAULT: release

both: release debug

ifdef DESTDIR
  # Debian uses this
  INSTALLPATH = $(DESTDIR)/usr
  SHAREPATH = $(DESTDIR)/usr/share
else
  INSTALLPATH = /usr/local
  SHAREPATH = /usr/local/share
endif

QMAKE=qmake
SELECTQT="-qt=qt5"


PVCAM_GENERATED=src/pvp/pvp_Class0.h src/pvp/pvp_Class2.h src/pvp/pvp_Class3.h \
	src/pvp/pvp_Class0.cpp src/pvp/pvp_Class2.cpp src/pvp/pvp_Class3.cpp \
	src/pvp/pvpDummy.cpp

NIDAQ_GENERATED=src/daq/daqdummy.cpp

GENERATED=$(PVCAM_GENERATED) $(NIDAQ_GENERATED) src/base/enums.h

COMMON=src/toplevel/version.xml src/vscope.pro src/vscope.pri

release: build/Makefile
	+make -C build release

build/Makefile:	$(PVCAM_GENERATED) $(COMMON)
	mkdir -p build
	( cd build; $(QMAKE) $(SELECTQT) ../src/vscope.pro )

debug: build-dbg/Makefile
	+make -C build-dbg debug

build-dbg/Makefile: $(PVCAM_GENERATED) $(COMMON)
	mkdir -p build-dbg
	( cd build-dbg; $(QMAKE) $(SELECTQT) ../src/vscope.pro )

clean:; rm -rf build build-dbg build-doc build-mtpsd

src/pvp/pvp_Class0.h src/pvp/pvp_Class2.h src/pvp/pvp_Class3.h \
src/pvp/pvp_Class0.cpp src/pvp/pvp_Class2.cpp src/pvp/pvp_Class3.cpp: \
	tools/pvcam2param.pl pvcam/pvcam.h
	$<

src/pvp/pvpDummy.cpp: tools/pvcam2dummy.pl pvcam/pvcam.h
	$<

src/toplevel/version.xml:; tools/updateversion.sh

src/daq/daqdummy.cpp: tools/daqmx2dummy.pl nidaq/NIDAQmx.h
	$<

src/base/enums.h: tools/enums2c.pl src/base/enums.xml 
	$<

DOC:;	mkdir -p build-doc
	cp doc/Makefile build-doc
	+make -C build-doc

MTPSD:	build-mtpsd/mtpsd.oct build-mtpsd/dpss.oct

build-mtpsd/mtpsd.oct build-mtpsd/dpss.oct:;
	mkdir -p build-mtpsd
	cp tools/mtpsd/Makefile build-mtpsd/
	+make SRC=../tools/mtpsd -C build-mtpsd oct

install: release DOC MTPSD
	install -d $(INSTALLPATH)/bin
	install build/vscope $(INSTALLPATH)/bin/vscope
	mkdir -p $(SHAREPATH)/octave/packages/vscope-1.0/private 
	cp $(wildcard octave/vscope-1.0/*.m) $(SHAREPATH)/octave/packages/vscope-1.0/
	cp $(wildcard octave/vscope-1.0/private/*.m) $(SHAREPATH)/octave/packages/vscope-1.0/private/
	mkdir -p $(SHAREPATH)/octave/packages/vscope-1.0/packinfo
	cp octave/vscope-1.0/packinfo/DESCRIPTION $(SHAREPATH)/octave/packages/vscope-1.0/packinfo/
	install -d $(SHAREPATH)/man/man1
	cp build-doc/vscope.1 $(SHAREPATH)/man/man1/vscope.1
	install -d $(SHAREPATH)/applications
	install tools/vscope.desktop $(SHAREPATH)/applications/vscope.desktop
	install -d $(SHAREPATH)/pixmaps
	cp tools/vscope.svg $(SHAREPATH)/pixmaps/vscope.svg
	install -d $(INSTALLPATH)/lib/x86_64-linux-gnu/octave/vscope-1.0
	cp build-mtpsd/bin/dpss.oct  $(INSTALLPATH)/lib/x86_64-linux-gnu/octave/vscope-1.0/
	cp build-mtpsd/bin/mtpsd.oct  $(INSTALLPATH)/lib/x86_64-linux-gnu/octave/vscope-1.0/
