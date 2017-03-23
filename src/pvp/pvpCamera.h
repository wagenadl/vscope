// pvp/pvpCamera.h - This file is part of VScope.
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

// pvpCamera.h

#ifndef PVPCAMERA_H

#define PVPCAMERA_H

#include <pvp/pvpBase.h>
#include <pvp/dwpvcam.h>

#include <QString>
#include <QVector>
#include <QMap>

class pvpCamera {
public:
  pvpCamera(QString camname);
  ~pvpCamera();
  short getHandle() const { return camh; }
  QString getDriverVersion();
  QString getCameraChipName();
  QString getSerialNumber() const;
  void reportStatus(); // writes to stdout
  void reportSpeeds();
public: // Class 0 parameters - public read access only!
#include <pvp/pvp_Class0.h>
public: // Class 2 parameters - public read access only!
#include <pvp/pvp_Class2.h>
public: // Class 3 parameters - public read access only!
#include <pvp/pvp_Class3.h>
public:
  static int pvpTrigMode(class CCDTrigMode const &tm);
  static rgn_type pvpRegion(class CCDRegion const &reg, class CCDBinning const &bin);
  static int32 bestExposureTime(int32 t_us);
  // Returns best available exposure time given requested duration
  // In practice, this rounds down to integer milliseconds.
  int32 pvpExposureTime(int32 t_us, int32 *reso_us_out=0);
  // returns exposure in internal units, and sets exposure resolution
  void setRegion(class CCDRegion const &reg);
  void setBinning(class CCDBin const &reg);
  int getStatus();
  int countPorts();
  int countSpeeds(int port);
  void setPortAndSpeed(int port, int spdidx);
  int readoutTime(int port, int spdidx);
  int readoutTime(); // for current port and spdidx
  size_t configFinite(rgn_type const &rgn, int trigmode,
		      int exposetime, int nframes);
  // Returns number of pixels in sequence. exposetime must be in units
  // of current resolution.
  size_t configContinuous(rgn_type const &rgn, int trigmode,
			  int exposetime);
  // Returns number of pixels in buffer. exposetime must be in units
  // of current resolution.
  void startFinite(uint16_t *dest); // caller is responsible for buffer size!
  void finishFinite(uint16_t *dest);
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
public:
  QMap<int, QString> enumeratePorts();
private:
  QMap<int, QString> getEnumeration(int param);
  void initializeResIndex();
private:
  QString camname;
  QString serno;
  QString camchipname;
  short camh;
  
  QVector<int32> expres;
};

#endif
