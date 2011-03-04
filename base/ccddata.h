// ccddata.h

#ifndef CCDDATA_H

#define CCDDATA_H

#include <base/types.h>
#include <base/transform.h>
#include <base/keyaccess.h>

class CCDData: public KeyAccess {
  Q_OBJECT;
public:
  CCDData(int serpix=512, int parpix=512, int nframes=1);
  virtual ~CCDData();
public:
  uint16_t *frameData(WriteKey *key, int frame=-1);
  bool reshape(int serpix, int parpix, int nframes, bool free=false);
  /*:F reshape
   *:D Reshapes the data buffer. This may claim more space if needed; it
       will not release any unneeded space unless FREE is set to true.
   *:A True if reallocation occurred.
   *:N Emits newData() unless a key has been checked out.
   */
  void setTimeBase(double t0_ms, double dt_ms);
  /*:F setTimeBase
   *:D Store timing information with the data.
   *:N Emits newData() unless a key has been checked out.
  */
  void setDataToCanvas(Transform const &t);
  /*:F setDataToCanvas
   *:D Specifies how the image should be placed on the global canvas.
   *:N Emits newData() unless a key has been checked out.
  */
public:
  uint16_t const *frameData(int frame=-1) const;
  /*:F frameData
   *:D Returns a pointer to the n-th frame.
       Throws an exception if a non-existing frame is accessed.
       Without an argument, returns a pointer to the first frame, and no
       exception is thrown even if there are zero frames.
   */
  Transform const &dataToCanvas() const { return t; }
  int getSerPix() const { return serpix; }
  int getParPix() const { return parpix; }
  int getNFrames() const { return nframes; }
  int getFramePix() const { return framepix; }
  int getTotalPix() const { return nframes*framepix; }
  double getT0ms() const { return t0_ms; }
  double getDTms() const { return dt_ms; }
private:
  int serpix;
  int parpix;
  int nframes;
  int framepix;
  int allocpix;
  uint16_t *data;
  double t0_ms, dt_ms;
  Transform t;
};


#endif
