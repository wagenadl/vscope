// colors.h

#ifndef COLORS_H

#define COLORS_H
#include <QString>
#include <QDomElement>
#include <QMap>
#include <QColor>

class Colors {
public:
  static void add(QDomElement elt);
  /*:F read
   *:D elt can either be of type "colors" or contain an element of
       type "colors".
  */
  static QColor find(QString name, QString dflt);
  static QColor find(QString name); // throws exception if not found
  static bool has(QString name);
private:
  Colors();
  ~Colors();
  QMap<QString, QColor> map;
  void readxml(QDomElement elt);
private:
  static Colors *colors();
};

#endif
