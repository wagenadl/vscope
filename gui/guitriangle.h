// guitriangle.h

#ifndef GUITRIANGLE_H

#define GUITRIANGLE_H

#include <QString>

class guiTriangle {
public:
  guiTriangle(class guiPage *parent);
  void activate(QString id);
  void deactivate();
  bool isActive() const;
  void render(class QPainter *);
private:
  class guiPage *parent;
  class guiButton *src;
  class guiPage *dst;
  QString id;
  QPolygon tri;
};

#endif
