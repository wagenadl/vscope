// pvpCamera.h

#ifndef PVPCAMERA_H

#define PVPCAMERA_H

#include <pvp/pvpBase.h>
#include <pvp/dwpvcam.h>

#include <QString>
#include <QVector>

class pvpCamera {
public:
  pvpCamera(QString camname) throw(pvpException);
  ~pvpCamera();
  short getHandle() const { return camh; }
  QString getDriverVersion() const;
  QString getCameraChipName() const;
  QString getSerialNumber() const;
  void reportStatus() throw(pvpException); // writes to stdout
  void reportSpeeds() throw(pvpException);
public: // Class 0 parameters - public read access only!
#include <pvp/pvp_Class0.h>
public: // Class 2 parameters - public read access only!
#include <pvp/pvp_Class2.h>
public: // Class 3 parameters - public read access only!
#include <pvp/pvp_Class3.h>
public:
  static int pvpTrigMode(class CCDTrigMode const &tm) throw(pvpException);
  static rgn_type pvpRegion(class CCDRegion const &reg, class CCDBinning const &bin) throw(pvpException);
  static int32 bestExposureTime(int32 t_us);
  // Returns best available exposure time given requested duration
  // In practice, this rounds down to integer milliseconds.
  int32 pvpExposureTime(int32 t_us, int32 *reso_us_out=0) throw(pvpException);
  // returns exposure in internal units, and sets exposure resolution
  void setRegion(class CCDRegion const &reg);
  void setBinning(class CCDBin const &reg);
  int getStatus() throw(pvpException);
  size_t configFinite(rgn_type const &rgn, int trigmode,
		      int exposetime, int nframes);
  // Returns number of pixels in sequence. exposetime must be in units
  // of current resolution.
  size_t configContinuous(rgn_type const &rgn, int trigmode,
			  int exposetime);
  // Returns number of pixels in buffer. exposetime must be in units
  // of current resolution.
  void startFinite(uint16_t *dest); // caller is responsible for buffer size!
  void startContinuous(uint16_t *destbuf, size_t npixinbuf);
  void stopContinuous();
  void abort();
  enum Status {
    NotActive,
    Acquiring,
    Completed,
    Failed
  };
  Status getContinuousStatus();
  Status getFiniteStatus();
  size_t nPixelsSoFarContinuous(size_t npix_in_buf);
  size_t nPixelsSoFarFinite();
  int haveNewFrame();
  // Returns +ve if yes, 0 if no, -ve if failed.
  // Only for cont. acq.
  uint16_t *getLatestFrame();
  // Only for cont. acq. Does not check if data actually available.
private:
  QString camname;
  QString serno;
  QString camchipname;
  QString drivervsn;
  short camh;
  
  QVector<int32> expres;
};

#endif
