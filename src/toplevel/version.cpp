// version.cpp

#include "version.h"

#ifndef gitREVNO
#define gitREVNO "?"
#endif
#ifndef gitDATE
#define gitDATE "?"
#endif
#ifndef gitVERSION
#define gitVERSION "?"
#endif
#ifndef gitYEAR
#define gitYEAR "?"
#endif

#define buildDATE __DATE__ ", " __TIME__

QString versionBanner(QString cls) {
  QString txt = gitVERSION;
  txt += cls;

  txt += "<p>(C) Daniel A. Wagenaar 2008&ndash;" gitYEAR;
  QString gd = gitDATE;
  gd.replace("..", ", ");
  gd.replace(".", " ");
  txt += "<p>Last commit: " + gd + "<br>";
  txt += "Build date: " buildDATE;
  txt += "<p>For more info: daw@caltech.edu";

  return txt;
}
