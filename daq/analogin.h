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
    explicit Channel(unsigned int c=0);
    QString name() const;
    bool operator==(Channel const &x) const { return chn==x.chn; }
    bool operator<(Channel const &x) const { return chn<x.chn; }
  private:
    unsigned int chn;
  };
signals:
  void dataAvailable(AnalogIn *src, int nscans); // This signal will only be emitted if dataAvailablePeriod is set to non-zero.
  void acquisitionEnded(AnalogIn *src, bool ok);
public:
  AnalogIn(QString id="");
  virtual ~AnalogIn();
  // It may be necessary to add a setTriggering func, just so we can pass that down to slaves?
  void addChannel(Channel ain);
  void removeChannel(Channel ain);
  void clearChannels();
  void setRange(Channel ain, double range_V);
  void setGround(Channel ain, enum Ground ground);
  enum Ground getGround(Channel ain);
  double getRange(Channel ain);
  bool hasChannel(Channel ain) const;
  int getNumChannels() const;
  Channel getChannelAt(int idx) const;
  void setAcqLength(int nscans); // set to zero for undefined length (cont. acq.)
  int getAcqLength() const;
  virtual void commit();
  virtual void uncommit();
  virtual void start();
  virtual int countScansSoFar();
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
  virtual void abort();
  virtual void stop();
  virtual void setFrequency(double hz) ;
  void attachDI(class DigitalIn *digin);
  void attachAO(class AnalogOut *analout);
  void detachDI();
  void detachAO();
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
  QString channelName(Channel const &c) const;
public: // for internal use
  virtual void callbackDone(int status);
  virtual void callbackEvery(int nscans);
  virtual char const *name() const;
};

#endif
