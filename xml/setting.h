// setting.h

#ifndef SETTING_H

#define SETTING_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QRect>

class Setting {
public:
  Setting(QString type="", QString enumname="");
  void newType(QString type, QString enumname); // this invalidates the current value
  Setting &operator=(Setting const &src);
  QString toString() const;
  int toInt() const;
  double toDouble() const;
  QRect toRect() const;
  bool toBool() const;
  void set(QString s); // throws an exception if conversion failed
  void setInt(int i); 
  void setDouble(double f);
  void setRect(QRect const &r);
private:
  QVariant value;
  QString valueType;
  class Enumerator *enumerator;
};

#endif
