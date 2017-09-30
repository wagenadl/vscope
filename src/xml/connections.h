// xml/connections.h - This file is part of VScope.
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

// connections.h

#ifndef CONNECTIONS_H

#define CONNECTIONS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDomElement>
#include <base/campair.h>
#include <base/transform.h>

namespace Connections {
  /*:C Connections
   *:D Maintains information about connected hardware.
   */

  class AIChannel {
    /*:C Connections::AIChannel
     *:D Maintains information about an AI channel on the DAQ6229
     */
  public:
    AIChannel(QString id="-");
  public:
    QString id;     // id of this channel (as in enum AICHAN)
    int line;       // line of this channel
    QString ground; // RSE, NRSE, or DIFF
    double range;   // in volts: maximum expected voltage at DAQ.
    double scale;   // This says by how much a DAQ value (in volts) has to be
                    // multiplied to get our desired units.
                    // For instance, if we are measuring 10Vm with a 1x probe,
                    // so DAQ voltage is 10 * Vm, scale would be 1000 mV/V / 10,
                    // i.e., 100.
    double offset;  // This offset, in our own units, is added after scaling.
    QString unit;   // e.g. "mV" or "pA".
  };

  class AOChannel {
    /*:C Connections::AOChannel
     *:D Maintains information about an AO channel on the DAQ6229
     */
  public:
    AOChannel(QString id="-");
  public:
    QString id;     // id of this channel (as in enum AOCHAN)
    int line;       // line of this channel
    bool stim;
    double scale;   // this says by how much a DAQ value (in volts) has to be
                    // multiplied to get our desired units.
                    // For instance, if our multiclamp produces 400 pA / V,
                    // unit could be "nA" and scale "0.4".
    QString unit;   // e.g. "mV" or "pA".
  };
  
  class DigiChannel {
  public:
    DigiChannel(QString id="-");
  public:
    QString id;
    int line;
    bool out;
    bool in;
    bool stim;
  };

  class CamCon {
  public:
    CamCon(QString id="-");
  public:
    QString id;     // id of this camera (as in enum CAMERAS)
    QString serno;  // serial number of this camera
    QString partnerid; // or null if solo camera
    QString lampid; // light source to be switched on for this camera or null 
                    // for a light source that is not s/w controlled
    QString shtrid; // shutter to be opened for this camera or null for none
    QString trigid; // trigger to acquire a frame for this camera
    int xpix, ypix; // number of pixels in full frame
    double focusexp_ms;
    bool isdonor;
    bool isacceptor;
    int order;
    bool exists;
    Transform placement;
  };
  
  extern void readXML(QDomElement doc);
  /*:F read
   *:D Given a <connections> element or a document containing a
       <connections> element, set up the connections database.
  */
  extern void writeXML(QDomElement doc);
  /*:F read
   *:D Writes the db back to a <connections> element or a <vsdScopeSettings>
       element.
  */
  extern AIChannel const *findpAI(QString id);
  /*:F findpAI
   *:D Returns a pointer to a channel definition given an ID.
   *:R Returns null if not found.
   */
  extern AIChannel const &findAI(QString id);
  /*:F findAI
   *:D Returns a reference to a channel definition given an ID.
   *:R Throws an exception if not found.
   */
  extern AOChannel const *findpAO(QString id);
  /*:F findpAO
   *:D Returns a pointer to a channel definition given an ID.
   *:R Returns null if not found.
   */
  extern AOChannel const &findAO(QString id);
  /*:F findAO
   *:D Returns a reference to a channel definition given an ID.
   *:R Throws an exception if not found.
   */
  extern CamCon const *findpCam(QString id);
  /*:F findpCam
   *:D Returns a pointer to a camera definition given an ID.
   *:R Returns null if not found.
   */
  extern CamCon const &findCam(QString id);
  /*:F findCam
   *:D Returns a reference to a camera definition given an ID.
   *:R Throws an exception if not found.
   */
  extern CamPair camPair(QString id);
  /*:F camPair
   *:D Constructs a CamPair object for a given camera.
   *:R Throws an exception if not found.
   */
  extern void markCameraExists(QString id, bool exists=true);
  /*:F markCameraExists
   *:D Notifies Connections that the named camera is up and running.
   */
  QStringList allCams();
  /*:F allCams
   *:D Returns a list of all camera IDs, even nonexisting cameras.
   *:N The order of this list is as per their "order" xml attribute.
  */
  QStringList donorCams();
  /*:F donorCams
   *:D Returns a list of all camera IDs that have their "role" set to "donor".
   *:N The order of this list is as per their "order" xml attribute.
  */
  QString leaderCamera();
  /*:F leaderCamera
   *:D Returns the ID of the "leader" camera. Cameras are considered in this
       order: 1. existing donor cameras
              2. existing other cameras
	      3. non-existing donor cameras
	      4. non-existing other cameras
  */
  CamPair leaderCamPair();
  
  DigiChannel const *findpDig(QString id);
  /*:F findpDig
   *:D Returns a pointer to a digital line definition given an ID.
   *:R Returns null if not found.
   */
  DigiChannel const &findDig(QString id);
  /*:F findDig
   *:D Returns a reference to a digital line definition given an ID.
   *:R Throws an exception if not found.
   */
  QStringList digiOutputLines();
  /*:F digiOutputLines
   *:D Returns a list of digital lines that are outputs according to
       our list of connections.
  */
  QStringList digiInputLines();
  /*:F digiInputLines
   *:D Returns a list of digital lines that are inputs according to
       our list of connections.
  */
  QStringList digiStimLines();
  /*:F digiStimLines
   *:D Returns a list of digital lines that are outputs meant for user-defined
       stimuli, according to our list of connections.
  */
  
  QStringList analogInputs();
  QStringList analogOutputs();
  QStringList analogStims();
  /*:F analogInputs, analogOutputs, analogStims
   *:D Return lists of all analog inputs and outputs
   *:N "Stims" are output channels that can be used for user-defined stimuli
   */

  QString deviceID();
  QString deviceSerNo();
  QString deviceType();
  /*:F deviceID, deviceSerNo, deviceType
   *:D Returns ID, serial number, and type of configured DAQ device
   *:N Returns empty string if not specified, which is the default.
   */
};
  
#endif
