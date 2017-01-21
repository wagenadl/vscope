DEFAULT: release

both: release debug

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

clean:; rm -rf build build-dbg

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
