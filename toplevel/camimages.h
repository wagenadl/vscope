// camimages.h

#ifndef CAMIMAGES_H

#define CAMIMAGES_H

#include <gfx/roiimages.h>

class CamImages: public ROIImages {
  Q_OBJECT;
public:
  CamImages();
  void setup();
  virtual ~CamImages();
public slots:
  void updateCameras();
private:
  void setCameras(QStringList ids);
};

#endif
