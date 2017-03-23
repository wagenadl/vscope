// pvp/camera.h - This file is part of VScope.
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

// Camera.h

#ifndef CAMERA_H

#define CAMERA_H

#include <base/exception.h>
#include <pvp/ccdconfig.h>
#include <QStringList>
#include <QVector>
#include <base/types.h>

class Camera {
public:
  Camera(QString camname);
  ~Camera();

  // Querying basic camera info
  QString getSerialNumber() const;
  QStringList cameraInfo();
  void fullReport();

  // Managing cameras by logical name
  QString getID() const; 
  void setID(QString newid);

  // Setting configuration
  CCDConfig const &getConfig() const;
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
  void finishFinite();

  // Querying current run
  bool isRunning();
  bool haveNewFrame();
  uint16_t const *getLatestFrame();
  bool hasCompleted();
  size_t nPixelsSoFar();
  static void closeDown();
private:
  QString id;
  class pvpCamera *pvpcam;
  CCDConfig cfg;
  double expose_ms;
  size_t npixels_for_seq;
  QVector<uint16_t> contBuffer;
  bool isrunning;
  uint16_t *mydest;
};

#endif
