ALL: release MTPSD

both: release debug

ifdef DESTDIR
  # Debian uses this
  INSTALLPATH = $(DESTDIR)/usr
  SHAREPATH = $(DESTDIR)/usr/share
  #VERSION:
  # This ensures version.xml is not modified by debian build.
  # I think that is a decent solution. Alternatively, version.xml
  # could be updated in the build/ tree, and version.qrc could
  # somehow fetch it from there. That would be even better,
  # but too tricky for the moment.
else
  INSTALLPATH = /usr/local
  SHAREPATH = /usr/local/share
  #VERSION:; tools/updateversion.sh
  # This ensures version.xml is updated by regular build
endif

QMAKE=qmake
SELECTQT="-qt=qt5"

DEB_HOST_MULTIARCH ?= $(shell dpkg-architecture -qDEB_HOST_MULTIARCH)

PVCAM_GENERATED=src/pvp/pvp_Class0.h src/pvp/pvp_Class2.h src/pvp/pvp_Class3.h \
	src/pvp/pvp_Class0.cpp src/pvp/pvp_Class2.cpp src/pvp/pvp_Class3.cpp \
	src/pvp/pvpDummy.cpp

NIDAQ_GENERATED=src/daq/daqdummy.cpp

GENERATED=$(PVCAM_GENERATED) $(NIDAQ_GENERATED) src/base/enums.h #src/toplevel/version.xml

COMMON=src/vscope.pro src/vscope.pri

release: build/Makefile
	rm -f build/release/version.o # force rebuild
	+make -C build release

build/Makefile:	$(GENERATED) $(COMMON) FORCE
	mkdir -p build
	( cd build; $(QMAKE) $(SELECTQT) ../src/vscope.pro )

debug: build-dbg/Makefile
	rm -f build-dbg/debug/version.o # force rebuild
	+make -C build-dbg debug

build-dbg/Makefile: $(GENERATED) $(COMMON) FORCE
	mkdir -p build-dbg
	( cd build-dbg; $(QMAKE) $(SELECTQT) ../src/vscope.pro )

clean:; rm -rf build build-dbg build-doc build-mtpsd

src/pvp/pvp_Class0.h src/pvp/pvp_Class2.h src/pvp/pvp_Class3.h \
src/pvp/pvp_Class0.cpp src/pvp/pvp_Class2.cpp src/pvp/pvp_Class3.cpp: \
	tools/pvcam2param.pl pvcam/pvcam.h
	$<

src/pvp/pvpDummy.cpp: tools/pvcam2dummy.pl pvcam/pvcam.h
	$<

#src/toplevel/version.xml:; tools/updateversion.sh

src/daq/daqdummy.cpp: tools/daqmx2dummy.pl nidaq/NIDAQmx.h
	$<

src/base/enums.h: tools/enums2c.pl src/base/enums.xml 
	$<

DOC:;	mkdir -p build-doc
	cp doc/Makefile build-doc
	+make -C build-doc

MTPSD:; mkdir -p build-mtpsd
	cp tools/mtpsd/Makefile build-mtpsd/
	+make SRC=../tools/mtpsd -C build-mtpsd oct bin/mkdpss

OCTAVEVSN=$(shell grep Version octave/vscope-1.0/packinfo/DESCRIPTION|cut -f2 -d' ')

install: release DOC MTPSD
	install -d $(INSTALLPATH)/bin
	install build/vscope $(INSTALLPATH)/bin/vscope
	install -d $(SHAREPATH)/octave/packages/vscope-1.0/private 
	install -m644 $(wildcard octave/vscope-1.0/*.m) $(SHAREPATH)/octave/packages/vscope-1.0
	install -m644 octave/vscope-1.0/PKG_ADD $(SHAREPATH)/octave/packages/vscope-1.0
	install -m644 octave/vscope-1.0/PKG_DEL $(SHAREPATH)/octave/packages/vscope-1.0
	install -m644 $(wildcard octave/vscope-1.0/private/*.m) $(SHAREPATH)/octave/packages/vscope-1.0/private
	install -d $(SHAREPATH)/octave/packages/vscope-$(OCTAVEVSN)/packinfo
	install -m644 octave/vscope-1.0/packinfo/DESCRIPTION $(SHAREPATH)/octave/packages/vscope-$(OCTAVEVSN)/packinfo
	install -d $(SHAREPATH)/man/man1
	install -m644 build-doc/vscope.1 $(SHAREPATH)/man/man1/vscope.1
	install -d $(SHAREPATH)/applications
	install tools/vscope.desktop $(SHAREPATH)/applications/vscope.desktop
	install -d $(SHAREPATH)/pixmaps
	install -m644 tools/vscope.svg $(SHAREPATH)/pixmaps/vscope.svg
	install -d $(INSTALLPATH)/lib/$(DEB_HOST_MULTIARCH)/octave/packages/vscope-$(OCTAVEVSN)
	install -m644 build-mtpsd/bin/dpss.oct  $(INSTALLPATH)/lib/$(DEB_HOST_MULTIARCH)/octave/packages/vscope-$(OCTAVEVSN)
	install -m644 build-mtpsd/bin/mtpsd.oct  $(INSTALLPATH)/lib/$(DEB_HOST_MULTIARCH)/octave/packages/vscope-$(OCTAVEVSN)
	install build-mtpsd/bin/mkdpss $(INSTALLPATH)/bin/mkdpss

FORCE:

zip:; ( cd octave; git archive -o /tmp/octave-vscope-`git describe`.zip HEAD vscope-1.0 ) && echo "Archive created in /tmp/octave-vscope-`git describe`.zip"
