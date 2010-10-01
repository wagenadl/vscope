// campool.h

#ifndef CAMPOOL_H

#define CAMPOOL_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QStringList>
#include <pvp/camera.h>

class CamPool {
public:
  CamPool();
public:
  static int nCameras();
  static Camera &get(int);
  static Camera &find(QString id); // initially, id is set to serial number
  static Camera *findp(QString id);
  static void rename(QString oldid, QString newid);
  static bool haveAll(QStringList const &ids);
  static QStringList systemInfo();
private:
  static void initialize();
private:
  static QMap<QString, Camera *> byId;
  static QVector<Camera *> cameras;
  static bool inited;
};

#endif
