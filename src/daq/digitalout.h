// digitalout.h

#ifndef DIGITALOUT_H

#define DIGITALOUT_H

#include <daq/daqtask.h>
#include <base/types.h>

class DigitalOut: public daqTask {
  Q_OBJECT;
signals:
  void productionEnded(DigitalOut *src, bool ok);
public:
  DigitalOut(class AnalogOut *master, QString id="") /*throw(daqException)*/;
  virtual ~DigitalOut();
  void setData(class DigitalData *ad) /*throw(daqException)*/; // Data must stay available from start to stop.
  // Data must be defined before calling commit or start.
  void setChannelMask(uint32_t mask) /*throw(daqException)*/;
  void addChannel(int lineno) /*throw(daqException)*/;
  void removeChannel(int lineno) /*throw(daqException)*/;
  uint32_t getChannelMask() const { return cmask; }
  bool hasChannel(int lineno) /*throw(daqException)*/;
  virtual void commit() /*throw(daqException)*/;
  virtual int countScansSoFar() /*throw(daqException)*/;
private:
  class AnalogOut *master;
  uint32_t cmask;
  class DigitalData *data;
  QVector<uint8_t> data8;
  QVector<uint16_t> data16;
private:
  void writeData() /*throw(daqException)*/;
public: // for internal use
  virtual void callbackDone(int status);
  virtual char const *name() const;
};


#endif
