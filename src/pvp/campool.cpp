// pvp/campool.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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

void CamPool::closedown() {
  Dbg() << "Campool closedown";
  foreach (Camera *c, cameras) {
    delete c;
  }
  cameras.clear();
  byId.clear();
  inited = false;
  Camera::closeDown();
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
  Dbg() << "Campool contents";
  foreach (QString s, byId.keys())
    Dbg() << "campool::findp byid=" << s;
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
  Dbg() << "campool rename" << oldid << newid << (findp(oldid) ? 1 : 0);
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

