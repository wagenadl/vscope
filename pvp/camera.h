// Camera.h

#ifndef CAMERA_H

#define CAMERA_H

#include <base/exception.h>
#include <pvp/ccdconfig.h>
#include <QStringList>
#include <base/types.h>

class Camera {
public:
  Camera(QString camname);
  ~Camera();

  // Querying basic camera info
  QString getSerialNumber() const;
  QStringList cameraInfo();

  // Managing cameras by logical name
  QString getID() const; 
  void setID(QString newid);

  // Setting configuration
  void setConfig(CCDConfig const &cfg);
  
  // Querying configured sequence
  size_t nPixelsForSeq() const;
  double getExposureMS() const;
  bool isContinuous() const;

  // Run control
  void startFinite(uint16_t *dest, size_t destsize_pix);
  void startContinuous();
  void stopContinuous();
  void abort();

  // Querying current run
  bool isRunning();
  bool haveNewFrame();
  uint16_t const *getLatestFrame();
  bool hasCompleted();
  size_t nPixelsSoFar();
private:
  QString id;
  class pvpCamera *pvpcam;
  CCDConfig cfg;
  double expose_ms;
  size_t npixels_for_seq;
  uint16_t *contBuffer;
  size_t npixels_in_buffer;
};

#endif
