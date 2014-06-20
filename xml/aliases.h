// aliases.h

#ifndef ALIASES_H

#define ALIASES_H

#include <QDomElement>
#include <QMap>
#include <QString>

class Aliases {
public:
  static void add(QDomElement def);
  static QString lookup(QString id);
  static bool has(QString id);
private:
  QMap<QString, QString> map;
  QMap<QString, QString> wildcarded;
  void readxml(QDomElement def);
private:
  Aliases();
  static Aliases &a();
};

#endif
