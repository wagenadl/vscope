#include <QGraphicsItem>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QImage>
#include <QPixmap>

#include "mygv.H"

#define FILENAME "/home/wagenaar/werk/presentations/swimcircuitry-drawings/ganglion-dorsal.png"

int main(int argc, char **argv) {
  QApplication qap(argc, argv);
  QImage img(FILENAME);
  QPixmap pxm = QPixmap::fromImage(img);

  QPolygonF blob;
  blob.push_back(QPointF(400,300)*3);
  blob.push_back(QPointF(430,300)*3);
  blob.push_back(QPointF(450,240)*3);
  blob.push_back(QPointF(430,180)*3);
  blob.push_back(QPointF(400,180)*3);
  blob.push_back(QPointF(300,150)*3);
  
  QGraphicsScene scene;
  QGraphicsPixmapItem *pxmi = scene.addPixmap(pxm);

  QPen pen;
  QBrush brush;
  pen.setColor(QColor(0,0,255,0));
  brush.setStyle(Qt::SolidPattern);
  brush.setColor(QColor(255,255,0,96));
  QGraphicsPolygonItem *polyi = scene.addPolygon(blob, pen, brush);
  
  QWidget mw;
  mw.setGeometry(200,150,512,512);
  mw.show();

  MyGV gv(&scene, &mw);
  gv.resize(mw.contentsRect().size());
  gv.show();
  gv.fitInView(pxmi, Qt::KeepAspectRatio);
  return qap.exec();
}


