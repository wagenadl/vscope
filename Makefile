DEFAULT: release

both: release debug

PVCAM_GENERATED=src/pvp/pvp_Class0.h src/pvp/pvp_Class2.h src/pvp/pvp_Class3.h \
	src/pvp/pvp_Class0.cpp src/pvp/pvp_Class2.cpp src/pvp/pvp_Class3.cpp \
	src/pvp/pvpDummy.cpp

NIDAQ_GENERATED=src/daq/daqdummy.cpp

GENERATED=$(PVCAM_GENERATED) $(NIDAQ_GENERATED)

COMMON=src/Makefile src/toplevel/version.xml

release: $(COMMON) src/Makefile.Release build/release $(GENERATED)
	+make -C src release

debug: $(COMMON) src/Makefile.Debug build/debug $(GENERATED)
	+make -C src debug

build/release:; mkdir -p $@
build/debug:; mkdir -p $@

clean:; rm -rf build/release build/debug

src/Makefile src/Makefile.Debug src/Makefile.Release build/release build/debug: \
	src/vscope.pro src/vscope.pri
	cd src; qmake -qt=qt5

src/pvp/pvp_Class0.h src/pvp/pvp_Class2.h src/pvp/pvp_Class3.h \
src/pvp/pvp_Class0.cpp src/pvp/pvp_Class2.cpp src/pvp/pvp_Class3.cpp: \
	tools/pvcam2param.pl pvcam/pvcam.h
	$<

src/pvp/pvpDummy.cpp: tools/pvcam2dummy.pl pvcam/pvcam.h
	$<

src/toplevel/version.xml: tools/updateversion.sh
	$<

src/daq/daqdummy.cpp: tools/daqmx2dummy.pl nidaq/NIDAQmx.h
	$<
