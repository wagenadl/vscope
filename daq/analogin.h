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
  class Channel {
  public:
    explicit Channel(unsigned int c);
    QString name() const;
    bool operator==(Channel const &x) { return chn==x.chn; }
  private:
    unsigned int chn;
  };
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
  int read(AnalogData *dest);
  /*:F read
   *:D Reads data into the specified destination. The number of scans to
       be read is determined by DEST's NumScans. At the end of an
       acquisition, there may be fewer scans actually available. DEST's
       NumScans will be updated to reflect this.
   *:R Number of scans actually read.
   *:N This will wait until data available, except at end of acq.
   *:N The channel map in DEST must match ours. We only verify that the number
       of channels matches.
   */
  int read(AnalogData *dest, int scanoffset, int scanlimit=0);
  /*:F read
   *:D Read a limited number of scans into an AnalogData structure, placing
       the result at the given offset.
   *:A If scanlimit=0, this will fill the rest of the buffer.
   *:R Number of scans read.
   *:N The AnalogData is *not* resized.
   *:N The channel map in DEST must match ours. We only verify that the number
       of channels matches.
   */
  virtual void abort() throw(daqException);
  virtual void stop() throw(daqException);
  virtual void setFrequency(double hz)  throw(daqException);
  void attachDI(class DigitalIn *digin) throw(daqException);
  void attachAO(class AnalogOut *analout) throw(daqException);
  void detachDI() throw(daqException);
  void detachAO() throw(daqException);
protected:
  int nscans;
  QVector<Channel> ai_channel_list; // by index
  QMap<Channel, double> ai_range_map; // by channel number
  QMap<Channel, enum Ground> ai_ground_map; // by channel number
  QMap<Channel, int> ai_channel2index;

  class DigitalIn *dislave;
  class AnalogOut *aoslave;
  bool aborting;
private:
  QString channelName(Channel const &c);
 public: // for internal use
  virtual void callbackDone(int status);
  virtual void callbackEvery(int nscans);
  virtual char const *name() const;
};

#endif
