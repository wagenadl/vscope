// analogout.h

#ifndef ANALOGOUT_H

#define ANALOGOUT_H

#include <daq/daqtask.h>
#include <QVector>
#include <QMap>

class AnalogOut: public daqTask {
  Q_OBJECT;
public:
  static const double AO_RANGE_V = 10.0;
signals:
  void dataNeeded(AnalogOut *src, int nscans);
  void productionEnded(AnalogOut *src, bool ok);
public:
  AnalogOut(class AnalogIn *master=0, QString id="");
  virtual ~AnalogOut();
  void setMaster(class AnalogIn *master=0);
  void setData(class AnalogData *ad, QList<int> const &channelmap);
  /* Data must stay available from start to stop.
     Data must be defined before calling commit or start. */
  void setContinuous(int bufsize=1024) throw(daqException);
  void setFiniteLength() throw(daqException);
  virtual void commit() throw(daqException);
  virtual void uncommit() throw(daqException);
  virtual void start() throw(daqException);
  virtual void stop() throw(daqException);
  virtual void abort() throw(daqException);
  void attachDO(class DigitalOut *slave) throw(daqException);
  void detachDO() throw(daqException);
  virtual int countScansSoFar() throw(daqException);
  virtual int countScansAvailable() throw(daqException); // returns space available
  virtual void setFrequency(double hz)  throw(daqException);
private:
  class AnalogIn *master;
  class DigitalOut *doslave;
  class AnalogData *data;
  bool isCont;
public:
  void writeData() throw(daqException);
  // Only to be used from within the dataNeeded callback.
public: // for internal use
  virtual void callbackEvery(int nscans);
  virtual void callbackDone(int status);
  virtual char const *name() const;
};


#endif
