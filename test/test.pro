TEMPLATE = subdirs
SUBDIRS = linegr.pro xmltest.pro sigslot.pro \
	guitest.pro keyboardtest.pro filedialog.pro pathbartest.pro \
	roi3d.pro ellipse.pro polyblobtest.pro ffttest.pro msgrecvtest.pro \
	cohtest.pro
 # soundtest.pro
 # typetest.pro
win32 {
  SUBDIRS+= daqtest.pro
}

