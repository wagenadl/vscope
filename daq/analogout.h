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
  class Channel {
  public:
    explicit Channel(unsigned int c=0);
    QString name() const;
    bool operator==(Channel const &x) const { return chn==x.chn; }
    bool operator<(Channel const &x) const { return chn<x.chn; }
  private:
    unsigned int chn;
  };
signals:
  void dataNeeded(AnalogOut *src, int nscans);
  void productionEnded(AnalogOut *src, bool ok);
public:
  AnalogOut(class AnalogIn *master=0, QString id="");
  virtual ~AnalogOut();
  void setMaster(class AnalogIn *master=0);
  void setData(class AnalogData *ad, QList<Channel> const &channelmap);
  /* Data must stay available from start to stop.
     Data must be defined before calling commit or start. */
  void setContinuous(int bufsize=1024);
  void setFiniteLength();
  virtual void commit();
  virtual void uncommit();
  virtual void start();
  virtual void stop();
  virtual void abort();
  void attachDO(class DigitalOut *slave);
  void detachDO();
  virtual int countScansSoFar();
  virtual int countScansAvailable(); // returns space available
  virtual void setFrequency(double hz) ;
private:
  class AnalogIn *master;
  class DigitalOut *doslave;
  class AnalogData *data;
  bool isCont;
  QList<Channel> channelmap;
public:
  void writeData();
  // Only to be used from within the dataNeeded callback.
private:
  QString channelName(Channel const &c) const;
public: // for internal use
  virtual void callbackEvery(int nscans);
  virtual void callbackDone(int status);
  virtual char const *name() const;
};


#endif
