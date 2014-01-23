// CohImages.h

#ifndef COHIMAGES_H

#define COHIMAGES_H

#include <gfx/cohmaps.h>

class CohImages: public CohMaps {
  Q_OBJECT;
public:
  CohImages(class CohData *data);
  void setup();
  virtual ~CohImages();
public slots:
  void updateCameras();
private:
  void setCameras(QStringList ids);
private:
  class CohData *data;
};

#endif
