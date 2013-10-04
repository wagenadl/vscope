// guitriangle.cpp

#include "guitriangle.h"
#include "guipage.h"
#include "guibutton.h"
#include <QPainter>
#include <base/dbg.h>
#include <QDebug>

guiTriangle::guiTriangle(guiPage *parent): parent(parent) {
  src = 0;
  dst = 0;
  id = "";
}

guiPage *guiTriangle::subpagep() {
  int idx = id.indexOf(ARRAYSEP);
  QString pid = idx>0 ? id.left(idx) : id;
  return parent->subpagep(pid);
}

guiButton *guiTriangle::buttonp() {
  return parent->buttonp(id);
}

void guiTriangle::activate(QString id0) {
  id = id0;
  
  guiButton *b = buttonp();
  guiPage *p = subpagep();

  if (!b || !p) {
    deactivate(id);
    return;
  }
  
  QRect br = b->geometry();
  QRect pr = p->geometry();
  bool butIsLeft = br.left()+br.right() < pr.left() + pr.right();
  int bx; // a bit in from near corner
  int px; // out one pixel from near corner
  int bx0; // near corner
  int bx1; // out one pixel from far corner
  int px1; // out one pixel far corner
  int byt = br.top() - 1;
  int byb = br.bottom() + 1;
  int pyt = pr.top() - 1;
  int pyb = pr.bottom() + 1;
  
  if (butIsLeft) {
    // button is to the left
    bx0 = br.right();
    bx1 = br.left() - 1;
    bx = bx0 - 12;
    px = pr.left() - 1;
    px1 = pr.right() + 1;
  } else {
    // button is to the right
    bx0 = br.left();
    bx1 = br.right() + 1;
    bx = bx0 + 12;
    px = pr.right() + 1;
    px1 = pr.left() - 1;
  }
  
  area = QPolygon(4);
  area.setPoint(0, bx, byt);
  area.setPoint(1, bx, byb);
  area.setPoint(2, px, pyb);
  area.setPoint(3, px, pyt);
  if (!area.containsPoint(QPoint(bx0, byt), Qt::OddEvenFill))
    area.setPoint(0, bx0, byt);
  if (!area.containsPoint(QPoint(bx0, byb), Qt::OddEvenFill))
    area.setPoint(1, bx0, byb);
  
  if (butIsLeft) {
    // darkline over left and top
    darkLine = QPolygon(5);
    darkLine.setPoint(0, bx1, byb); // button bl
    darkLine.setPoint(1, bx1, byt); // button tl
    darkLine.setPoint(2, area.point(0)); // corner
    darkLine.setPoint(3, px, pyt); // subpage tl
    darkLine.setPoint(4, px1, pyt); // subpage tr
    // lightline over bottom and right
    lightLine = QPolygon(5);
    lightLine.setPoint(0, bx1, byb); // button bl
    lightLine.setPoint(1, area.point(1)); // corner
    lightLine.setPoint(2, px, pyb); // subpage bl
    lightLine.setPoint(3, px1, pyb); // subpage br
    lightLine.setPoint(4, px1, pyt); // subpage tr
  } else {
    // darkline over top and left
    darkLine = QPolygon(5);
    darkLine.setPoint(0, bx1, byt);
    darkLine.setPoint(1, area.point(0));
    darkLine.setPoint(2, px, pyt);
    darkLine.setPoint(3, px1, pyt);
    darkLine.setPoint(4, px1, pyb);
    // lightline over right and bottom
    lightLine = QPolygon(5);
    lightLine.setPoint(0, bx1, byt);
    lightLine.setPoint(1, bx1, byb);
    lightLine.setPoint(2, area.point(1));
    lightLine.setPoint(3, px, pyb);
    lightLine.setPoint(4, px1, pyb);
  }      

  recolor();
}

void guiTriangle::recolor() {
  guiButton *b = buttonp();
  guiPage *p = subpagep();

  if (!b || !p) {
    deactivate(id);
    return;
  }

  QColor mid = b->palette().color(QPalette::Mid);
  QColor but = b->palette().color(QPalette::Button);
  if (b->getVisualType()==VT_ArrayCtrl) {
    areaColor = mid;
    QPalette pal = p->palette();
    pal.setColor(QPalette::Window, mid);
    p->setPalette(pal);
  } else {
    areaColor = but;
  }
  
  lightColor = QColor(b->palette().color(QPalette::Light));
  darkColor = QColor(b->palette().color(QPalette::Dark));

  parent->update();
}  

void guiTriangle::deactivate(QString id0) {
  if (id0!=id)
    return;
  
  id = "";
  parent->update();
}

bool guiTriangle::isActive() const {
  return id != "";
}

void guiTriangle::render() {
  if (!isActive())
    return;

  QPainter p(parent);
  p.setPen(QPen(areaColor));
  p.setBrush(QBrush(areaColor));
  qDebug() << p.brush();
  p.drawPolygon(area, Qt::WindingFill);

  p.setPen(lightColor);
  p.drawPolyline(lightLine);

  p.setPen(darkColor);
  p.drawPolyline(darkLine);
}
