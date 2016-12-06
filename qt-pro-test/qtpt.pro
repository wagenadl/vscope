message("Make spec:")
message($$QMAKESPEC)
message("Config:")
message($$CONFIG)
message("Defines:")
message($$DEFINES)

linux-g++ { message(Linux g++) }
cygwin-g++ { message(Cygwin g++) }
unix { message(Unix) }
linux { message(Linux) }
win32 { message(Win32) }
cygwin { message(Cygwin) }


LIBS += -L..\pvcam\i386 -lpvcam32
SOURCES += main.cpp \
    main.cpp
