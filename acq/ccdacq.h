// ccdacq.h

#ifndef CCDACQ_H

#define CCDACQ_H

#include <QVector>
#include <QMap>
#include <QString>
#include <pvp/ccdconfig.h>
#include <xml/connections.h>

class CCDAcq {
public:
  CCDAcq();
  ~CCDAcq();
  bool prepare(class ParamTree /*const*/ *ptree, bool snap);
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
  int nPixelsSoFar(QString camid);
  /*:F nPixelsSoFar
   *:D Returns number of pixels transferred so far by the given camera.
   */
private:
  QVector<QString> camids;
  QVector<Connections::CamCon const *> caminfo;
  QVector<class CCDData *> dest;
  QVector<CCDConfig> ccdcfg;
  QVector<class Camera *> cameras;
  QMap<QString, int> camidx;
  int ncams;

  bool isActive;
  bool isGood;
  bool isDone;

  double t0_ms, dt_ms; // expected timing of frames relative to trial
};

#endif
