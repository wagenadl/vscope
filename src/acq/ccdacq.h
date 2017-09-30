// acq/ccdacq.h - This file is part of VScope.
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

// ccdacq.h

#ifndef CCDACQ_H

#define CCDACQ_H

#include <QVector>
#include <QMap>
#include <QString>
#include <pvp/ccdconfig.h>
#include <xml/connections.h>
#include <acq/allccdtimingdetail.h>
#include <base/keyaccess.h>

#ifdef vsdLINUX
#define CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA 1
#else
#define CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA 0
#endif

class CCDAcq {
public:
  CCDAcq();
  ~CCDAcq();
  bool prepare(class ParamTree const *ptree, AllCCDTimingDetail const &timing);
  /*:F prepare
   *:D Gets the cameras ready.
   *:R Returns true iff the cameras are present and available.
   */
  void setDest(QString camid, class CCDData *dest);
  /*:F setDest
   *:D We do not do our own memory management; this specifies where the
       data for the next run should go.
  */
  void start();
  /*:F start
   *:D Starts the acquisition process.
       Actual image acquisition will wait for triggers, but this
       function returns immediately.
   *:N This will reshape the two destination buffers as needed.
  */
  void abort();
  void finish();
  bool hasEnded();
  /*:F hasEnded
   *:D Returns true iff the acquisition has finished, whether successful or
       not. This function returns quickly if the answer is "no", so it can
       be used to poll for completion. (Unfortunately, the PVCAM library
       does not support event-based completion notification, so polling is
       a sad necessity.) If the acquisition is complete, this function also
       relinquishes the exclusive ownership of the cameras.
   *:N Use wasSuccessful() to test whether acquisition ended well.
   */
  bool wasSuccessful();
  /*:F wasSuccessful
   *:D If hasEnded() has indicated that acquisition has ended, use this
       function to check if it ended OK.
  */
  size_t nPixelsSoFar(QString camid);
  /*:F nPixelsSoFar
   *:D Returns number of pixels transferred so far by the given camera.
   */
  Transform placement(QString camid) const;
  /*:F placement
   *:D Returns a transformation matrix that says where in canvas space
       the image of a given camera lies. This is only valid after prepare().
   *:N Returns a unit Transform if the camera is not in the set of cameras.
   */
private:
  QStringList camids;
  QMap<QString, Connections::CamCon const *> caminfo;
  QMap<QString, class CCDData *> dest;
  QMap<QString, CCDConfig> ccdcfg;
  QMap<QString, class Camera *> cameras;

  bool isActive;
  bool isGood;
  bool isDone;

  int t0_us, dt_us, frdur_us; // expected timing of frames relative to trial
  QMap<QString, KeyAccess::WriteKey *> keys;
};

#endif
