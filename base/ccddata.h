// ccddata.h

#ifndef CCDDATA_H

#define CCDDATA_H

#include <base/types.h>

class CCDData_ {
protected:
  int serpix;
  int parpix;
  int nframes;
  int framepix;
  int allocpix;
  uint16_t *data;
  double t0_ms, dt_ms;
};

class CCDData: private CCDData_ {
public:
  CCDData(int serpix=512, int parpix=512, int nframes=1);
  ~CCDData();
  CCDData(CCDData const &other);
  CCDData &operator=(CCDData const &other);
  bool reshape(int serpix, int parpix, int nframes, bool free=false);
  /*:F reshape
   *:D Reshapes the data buffer. This may claim more space if needed; it
       will not release any unneeded space unless FREE is set to true.
   *:A True if reallocation occurred.
   */
  uint16_t const *frameData(int frame=-1) const;
  uint16_t *frameData(int frame=-1);
  int getSerPix() const { return serpix; }
  int getParPix() const { return parpix; }
  int getNFrames() const { return nframes; }
  int getFramePix() const { return framepix; }
  int getTotalPix() const { return nframes*framepix; }
  void setTimeBase(double t0_ms, double dt_ms);
  /*:F setTimeBase
   *:D Store timing information with the data.
  */
  double getT0ms() const { return t0_ms; }
  double getDTms() const { return dt_ms; }
private:
  void copy(CCDData const &other);
private:
  int serpix;
  int parpix;
  int nframes;
  int framepix;
  int allocpix;
  uint16_t *data;
  double t0_ms, dt_ms;
};

#endif
