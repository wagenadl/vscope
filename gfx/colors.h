// colors.h

#ifndef COLORS_H

#define COLORS_H
#include <QString>
#include <QDomElement>
#include <QMap>
#include <QColor>

class Colors {
public:
  Colors(QDomElement elt);
  ~Colors();
  static QColor find(QString name, QString dflt);
  static QColor find(QString name); // throws exception if not found
  static bool has(QString name);
private:
  void read(QDomElement elt);
  /*:F read
   *:D elt can either be of type "colors" or contain an element of
       type "colors".
  */
  QMap<QString, QColor> map;
  static Colors *colors;
};

#endif
