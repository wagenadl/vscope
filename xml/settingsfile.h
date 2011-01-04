// settingsfile.h

#ifndef SETTINGSFILE_H

#define SETTINGSFILE_H

#include <QString>
#include <xml/paramtree.h>

class SettingsFile {
public:
  static void save(QString fn, ParamTree const *src);
  static void load(QString fn, ParamTree *dst);
};

#endif
