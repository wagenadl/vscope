// campool.cpp

#include "campool.h"
#include <pvp/pvpSystem.h>
#include <base/dbg.h>

QMap<QString, Camera *> CamPool::byId;
QVector<Camera *> CamPool::cameras;
bool CamPool::inited = false;


void CamPool::initialize() {
  if (inited)
    return;
  pvpSystem();
  int N = pvpSystem::countCameras();
  for (int n=0; n<N; n++) {
    QString name = pvpSystem::getCamName(n);
    Dbg() << "Camera #" << n << ": " << name;
    Camera *cam = new Camera(name);
    Dbg() << "  Serial no: " << cam->getSerialNumber();
    cameras.push_back(cam);
    byId[cam->getSerialNumber()] = cam;
  }
  inited = true;
}

int CamPool::nCameras() {
  return cameras.size();
}

Camera &CamPool::get(int n) {
  if (n>=0 && n<CamPool::cameras.size())
    return *cameras[n];
  else
    throw Exception("CamPool","No such camera");
}

Camera &CamPool::find(QString serno) {
  if (byId.contains(serno)) {
    Camera *c = byId[serno];
    if (c)
      return *c;
    else
      throw Exception("CamPool", "Null camera: " + serno);
  } else {
    throw Exception("CamPool", "No such camera: " + serno);
  }
}

Camera *CamPool::findp(QString serno) {
  if (byId.contains(serno))
    return byId[serno];
  else
    return 0;
}

bool CamPool::haveAll(QStringList const &cams) {
  try {
    foreach (QString s, cams) {
      if (!byId.contains(s))
	return false;
    }
    return true;
  } catch (Exception const &) {
    return false;
  }
}

void CamPool::rename(QString oldid, QString newid) {
  Camera *c = findp(oldid);
  if (c)
    c->setID(newid);
  byId[newid] = c;
}

QStringList CamPool::systemInfo() {
  QStringList res;
  res.append("Cameras");
  res.append(QString::number(pvpSystem::countCameras()));
  res.append("PVCam version");
  int v = pvpSystem::getPVCAMVersion();
  res.append(v>=0
             ? QString("%1.%2.%3").arg((v>>8)&0xff).arg((v>>4)&0x0f).arg(v&0x0f)
             : "none");
  return res;
}

