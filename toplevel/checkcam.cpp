// checkcam.cpp

#include <QWidget>
#include <stdio.h>

#include <QLabel>
#include <QStringList>
#include <QList>

#include <base/dbg.h>
#include <pvp/campool.h>
#include <pvp/camera.h>
#include <toplevel/globals.h>
#include <xml/connections.h>

static QLabel *lbl = 0;
static QList<QLabel *> camlbls;

QStringList checkcam() {
  QStringList list;
  try {
    CamPool::initialize();
    QString s="";
    QStringList sl = CamPool::systemInfo();
    while (!sl.isEmpty()) {
      QString k = sl.takeFirst();
      QString v = sl.takeFirst();
      s += k + ": " + v + "\n";
    }
    list.append(s);
    
    int ncams = CamPool::nCameras();

    QStringList camids = Connections::allCams();
    QMap<QString,QString> serno2id;
    for (QStringList::iterator i=camids.begin(); i!=camids.end(); ++i) 
      serno2id[Connections::findCam(*i).serno] = *i;
    
    for (int i=0; i<ncams; i++) {
      Camera &cam = CamPool::get(i);
      QString serno = cam.getSerialNumber();
      bool usingthis = serno2id.contains(serno);
      QString id = usingthis ? serno2id[serno] : "-";
      QString lbl = usingthis ? "\"" + id + "\""
	: "#" + QString::number(i);
      QString s = "Camera " + lbl + "\n";
      QStringList sl = cam.cameraInfo();
      while (!sl.isEmpty()) {
	QString k = sl.takeFirst();
	QString v = sl.takeFirst();
	s += k + ": " + v + "\n";
      }
      if (usingthis) {
	Connections::CamCon const &cam = Connections::findCam(id);
	s += QString("Size: %1 x %2 px\n").arg(cam.xpix).arg(cam.ypix);
	s += "Placement: " + cam.placement.stringRep() + "\n";
	s += QString("Expose: %1 ms").arg(cam.focusexp_ms);
      }
      list.append(s);
    }
  } catch (Exception const &e) {
    QString s = "Cannot read camera info";
    list.append(s);
  }
  return list;
}

void checkcam(QWidget *dest) {
  int hei = dest->height();
  int wid = dest->width();
  if (!lbl)
    lbl = new QLabel(dest);
  lbl->setGeometry(10,10,wid-20, 25);
  lbl->show();

  QStringList list = checkcam();

  QString s = list.takeFirst();
  s.replace("\n","  *  ");
  lbl->setText(s);
  
  int ncams = list.size();
  while (camlbls.size()>ncams) 
    delete camlbls.takeLast();
  while (camlbls.size()<ncams) 
    camlbls << new QLabel(dest);

  for (int i=0; i<ncams; i++) {
    camlbls[i]->setGeometry(10+i*(wid-20)/ncams,40,(wid-20)/ncams, hei-50);
    camlbls[i]->setText(list[i]);
    camlbls[i]->show();
  }
}
