// pvp/campool.h - This file is part of VScope.
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

// campool.h

#ifndef CAMPOOL_H

#define CAMPOOL_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QStringList>
#include <pvp/camera.h>

class CamPool {
private:
  CamPool();
public:
  static int nCameras();
  static Camera &get(int);
  static Camera &find(QString id); // initially, id is set to serial number
  static Camera *findp(QString id);
  static void rename(QString oldid, QString newid);
  static bool haveAll(QStringList const &ids);
  static QStringList systemInfo();
  static void closedown();
public:
  static void initialize();
private:
  static QMap<QString, Camera *> byId;
  static QVector<Camera *> cameras;
  static bool inited;
};

#endif
