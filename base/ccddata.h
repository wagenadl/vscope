// ccddata.h

#ifndef CCDDATA_H

#define CCDDATA_H

#include <base/types.h>
#include <base/transform.h>
#include <QObject>

class CCDData: public QObject {
  Q_OBJECT;
public:
  CCDData(int serpix=512, int parpix=512, int nframes=1);
  virtual ~CCDData();
public:
  class CheckoutKey { };
  CheckoutKey checkout();
  void checkin(CheckoutKey);
  void cancel(CheckoutKey);
  uint16_t *frameData(CheckoutKey, int frame=-1);
  bool reshape(int serpix, int parpix, int nframes, bool free=false);
  /*:F reshape
   *:D Reshapes the data buffer. This may claim more space if needed; it
       will not release any unneeded space unless FREE is set to true.
   *:A True if reallocation occurred.
   */
  void setTimeBase(double t0_ms, double dt_ms);
  /*:F setTimeBase
   *:D Store timing information with the data.
  */
  void setDataToCanvas(Transform const &t);
public:
  Transform const &dataToCanvas() const { return t; }
  uint16_t const *frameData(int frame=-1) const;
  int getSerPix() const { return serpix; }
  int getParPix() const { return parpix; }
  int getNFrames() const { return nframes; }
  int getFramePix() const { return framepix; }
  int getTotalPix() const { return nframes*framepix; }
  double getT0ms() const { return t0_ms; }
  double getDTms() const { return dt_ms; }
signals:
  void newData();
private:
  int serpix;
  int parpix;
  int nframes;
  int framepix;
  int allocpix;
  uint16_t *data;
  double t0_ms, dt_ms;
  Transform t;
  bool checkedout;
};

class CCDDataWriter {
public:
  CCDDataWriter(CCDData &data);
  ~CCDDataWriter();
  uint16_t *frameData(int frame=-1);
  void cancel();
private:
  CCDData &data;
  CCDData::CheckoutKey key;
  bool canceled;
};

#endif
