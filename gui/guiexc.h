// exception.h

#ifndef GUIEXCEPTION_H

#define GUIEXCEPTION_H

#include <QString>
#include <base/exception.h>

class GUIExc {
public:
  static void report(Exception const &e, QString reporter) throw();
  // report calls saveSettings
  static void warn(QString txt) throw();
  // warn does not call saveSettings
public:
  static void setSettingsDir(QString path);
  static void setParamTree(class ParamTree const *ptree);
  static void setArgs(int argc, char **argv);
  static void saveSettings();
private:
  static QString settingsdir;
  static class ParamTree const *ptree;
  static int argc;
  static char **argv;
};

#endif
