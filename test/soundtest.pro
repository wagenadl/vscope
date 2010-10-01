TEMPLATE = app
win32 {
  include(win.pro)
}
unix {
  include(../config.pro)
}

TARGET=soundtest
SOURCES=soundtest.cpp
