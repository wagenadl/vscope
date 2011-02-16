// campool.cpp

#include "campool.h"
#include <pvp/pvpSystem.h>
#include <base/dbg.h>

QMap<QString, Camera *> CamPool::byId;
QVector<Camera *> CamPool::cameras;
bool CamPool::inited = false;

CamPool::CamPool() {
  initialize();
}

void CamPool::initialize() {
  if (inited)
    return;
  Dbg() << "Initializing camera pool";
  pvpSystem();
  int N = pvpSystem::countCameras();
  Dbg() << "Counted cameras: " << N;
  for (int n=0; n<N; n++) {
    QString name = pvpSystem::getCamName(n);
    Dbg() << "Camera #" << n << ": " << name;
    Camera *cam = new Camera(name);
    Dbg() << "  Camera " << n << " initialized";
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
  if (byId.contains(serno))
    return *byId[serno];
  else
    throw Exception("CamPool","No such camera");
}

Camera *CamPool::findp(QString serno) {
  if (byId.contains(serno))
    return byId[serno];
  else
    return 0;
}

bool CamPool::haveAll(QStringList const &cams) {
  try {
    Dbg() << "CamPool count: " << nCameras();
    for (int i=0; i<nCameras(); i++)
      Dbg() << "  Camera #" << i << ": '" << cameras[i]->getID()
	    << "' (#" << cameras[i]->getSerialNumber() << ")";
    foreach (QString s, cams) {
      Dbg() << "CamPool looking for '" << s << "'";
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
  res.append("Driver vsn");
  int v = pvpSystem::getDriverVersion();
  res.append(v>=0 ? QString::number(v) : "none");
  res.append("PVCam vsn");
  v = pvpSystem::getPVCAMVersion();
  res.append(v>=0 ? QString::number(v) : "none");
  return res;
}

static CamPool pool_; // ensure initialization
