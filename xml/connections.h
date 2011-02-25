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
    double scale;   // this says by how much a DAQ value (in volts) has to be
                    // multiplied to get our desired units.
                    // For instance, if we are measuring 10Vm with a 1x probe,
                    // so DAQ voltage is 10 * Vm, scale would be 1000 mV/V / 10,
                    // i.e., 100.
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
  };

  class CamCon {
  public:
    CamCon(QString id="-");
  public:
    QString id;     // id of this camera (as in enum CAMERAS)
    QString serno;  // serial number of this camera
    QString partnerid;
    int xpix, ypix; // number of pixels in full frame
    bool flipx, flipy; // true if this image should be flipped
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
};
  
#endif
