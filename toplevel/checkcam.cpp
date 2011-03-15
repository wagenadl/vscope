// checkcam.cpp

#include <QWidget>
#include <stdio.h>

#include <QLabel>
#include <QStringList>

#include <base/dbg.h>
#include <pvp/campool.h>
#include <pvp/camera.h>
#include <toplevel/globals.h>
#include <xml/connections.h>
#include <base/memalloc.h>

static QLabel *lbl = 0;
static QLabel **camlbls = 0;
static int ncameras = 0;

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
	s += QString("Flip:");
	if (cam.flipx)
	  s += " X";
	if (cam.flipy)
	  s += " Y";
	if (!cam.flipx && !cam.flipy)
	  s += " none";
	s += "\n";
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
  if (ncams != ncameras) {
    if (camlbls)
      delete [] camlbls;
    if (ncams>0) {
      camlbls = memalloc<QLabel*>(ncams, "checkcam");
      for (int i=0; i<ncams; i++)
	camlbls[i]=0;
    }
    ncameras = ncams;
  }

  for (int i=0; i<ncams; i++) {
    if (!camlbls[i])
      camlbls[i] = new QLabel(dest);
    camlbls[i]->setGeometry(10+i*(wid-20)/ncams,40,(wid-20)/ncams, hei-50);
    QString s = list.takeFirst();
    camlbls[i]->setText(s);
    camlbls[i]->show();
  }
}
