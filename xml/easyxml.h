// easyxml.h

#ifndef EASYXML_H

#define EASYXML_H

#include <QDomElement>
#include <QStringList>
#include <QPoint>
#include <QSize>

class EasyXML {
public:
  EasyXML();
  EasyXML(QDomElement elt);
  EasyXML(QDomElement elt, QString tag); // elt, or first child matching tag
  EasyXML(EasyXML const &easy, QString tag); // ditto
  bool isValid() const { return !elt.isNull(); }
  QString tagName() const { return elt.tagName(); }
  QStringList attributes();
  QString operator[](QString) const; // reads an attribute
  QString string(QString) const; // reads an attribute as a string
  int integer(QString, bool *ok=0) const; // reads an attribute as a number
  double real(QString, bool *ok=0) const; // reads an attribute as a number
  QSize size(QString, QString, bool *ok=0) const; // reads 2 attributes as size
  QPoint point(QString, QString, bool *ok=0) const; // reads 2 attributes as pt
  bool contains(QString) const;
  QList<EasyXML> children(); // all children
  QList<EasyXML> children(QString); // children with given tag
private:
  QDomElement elt;
};

#endif
