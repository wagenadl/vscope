// guitriangle.h

#ifndef GUITRIANGLE_H

#define GUITRIANGLE_H

#include <QString>
#include <QObject>
#include <QPolygon>
#include <QColor>

class guiTriangle: public QObject {
  Q_OBJECT;
public:
  guiTriangle(class guiPage *parent);
  void activate(QString id);
  void deactivate(QString id);
  bool isActive() const;
  QString currentId() const { return id; }
  void render();
private:
  class guiPage *parent;
  class guiButton *src;
  class guiPage *dst;
  QString id;
  QPolygon area;
  QPolygon darkLine;
  QPolygon lightLine;
  QColor areaColor;
  QColor darkColor;
  QColor lightColor;
};

#endif
