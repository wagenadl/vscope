// analogin.h

#ifndef ANALOGIN_H

#define ANALOGIN_H

#include <base/analogdata.h>
#include <daq/daqtask.h>
#include <QVector>
#include <QMap>


class AnalogIn: public daqTask {
  Q_OBJECT;
public:
  enum Ground { NRSE, RSE, DIFF };
signals:
  void dataAvailable(AnalogIn *src, int nscans); // This signal will only be emitted if dataAvailablePeriod is set to non-zero.
  void acquisitionEnded(AnalogIn *src, bool ok);
public:
  AnalogIn(QString id="") throw(daqException);
  virtual ~AnalogIn();
  // It may be necessary to add a setTriggering func, just so we can pass that down to slaves?
  void addChannel(int ain) throw(daqException);
  void removeChannel(int ain) throw(daqException);
  void clearChannels() throw(daqException);
  void setRange(int ain, double range_V);
  void setGround(int ain, enum Ground ground);
  enum Ground getGround(int ain) throw(daqException);
  double getRange(int ain) throw(daqException);
  bool hasChannel(int ain) const;
  int getNumChannels() const;
  int getChannelAt(int idx) const throw(daqException);
  void setAcqLength(int nscans); // set to zero for undefined length (cont. acq.)
  int getAcqLength() const;
  virtual void commit() throw(daqException);
  virtual void uncommit() throw(daqException);
  virtual void start() throw(daqException);
  virtual int countScansSoFar() throw(daqException);
  int read(AnalogData *dest) throw(daqException);
  /*:F read
   *:D Reads data into the specified destination. The number of scans to
       be read is determined by DEST's NumScans. At the end of an
       acquisition, there may be fewer scans actually available. DEST's
       NumScans will be updated to reflect this.
   *:R Number of scans actually read.
   *:N This will wait until data available, except at end of acq.
   */
  AnalogData *read(int nscanslimit) throw(daqException);
  /*:F read
   *:D Reads all currently available data into a newly allocated structure.
   *:N The caller is responsible for eventually deleting the AnalogData
       structure.
   */
  int read(AnalogData *dest, int scanoffset, int scanlimit=0) throw(daqException);
  /*:F read
   *:D Read a limited number of scans into an AnalogData structure, placing
       the result at the given offset.
   *:A If scanlimit=0, this will fill the rest of the buffer.
   *:R Number of scans read.
   *:N The AnalogData is *not* resized.
   */
  virtual void abort() throw(daqException);
  virtual void stop() throw(daqException);
  virtual void setFrequency(double hz)  throw(daqException);
  //virtual void setPollPeriod(int nscans) throw(daqException);
  ///*:F setPollPeriod
  // *:D See daqtask.h.
  // *:N This version rounds down the period so that it divides the DMA buffer
  //     size evenly. For this to work, you must call this function *after*
  //     commit()ing.
  //*/
  void attachDI(class DigitalIn *digin) throw(daqException);
  void attachAO(class AnalogOut *analout) throw(daqException);
  void detachDI() throw(daqException);
  void detachAO() throw(daqException);
protected:
  int nscans;
  QVector<int> ai_channel_list; // by index
  QMap<int, double> ai_range_map; // by channel number
  QMap<int, enum Ground> ai_ground_map; // by channel number
  QMap<int, int> ai_channel2index;

  class DigitalIn *dislave;
  class AnalogOut *aoslave;
  bool aborting;

 public: // for internal use
  virtual void callbackDone(int status);
  virtual void callbackEvery(int nscans);
  virtual char const *name() const;
};

#endif
