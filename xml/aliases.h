// aliases.h

#ifndef ALIASES_H

#define ALIASES_H

#include <QDomElement>
#include <QMap>
#include <QString>

class Aliases {
public:
  static void read(QDomElement def);
  static QString lookup(QString id);
  static bool has(QString id);
private:
  static QMap<QString, QString> map;
  static QMap<QString, QString> wildcarded;
};

#endif
