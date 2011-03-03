// ccddata.h

#ifndef CCDDATA_H

#define CCDDATA_H

#include <base/types.h>
#include <base/transform.h>
#include <QObject>
#include <QSet>

class CCDData: public QObject {
  Q_OBJECT;
public:
  CCDData(int serpix=512, int parpix=512, int nframes=1);
  virtual ~CCDData();
public:
  class CheckoutKey;
  CheckoutKey *checkout();
  void checkin(CheckoutKey *);
  void cancel(CheckoutKey *);
  /*:F checkout, checkin, cancel
   *:D To gain write access to the frame data, you must request a key using
       checkout(). The contents of the key is opaque. Once you're done writing,
       call checkin() with the same key. If you decide not to write, call
       cancel().
       newData() is emitted upon final checkin.
   *:N Slightly counterintuitively, if two clients check out, and one checks
       in first after which the other cancels, a signal is emitted at the
       time the second client cancels.
   *:N Do *not* delete keys yourself. Call either checkin or cancel.
   *:N checkout() does not guarantee exclusive access.
   */
  uint16_t *frameData(CheckoutKey *, int frame=-1);
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
signals:
  void newData();
private:
  void emitUnlessCheckedOut();
private:
  int serpix;
  int parpix;
  int nframes;
  int framepix;
  int allocpix;
  uint16_t *data;
  double t0_ms, dt_ms;
  Transform t;
  QSet<CheckoutKey *> keys;
  bool mustEmit;
};

class CCDDataWriter {
  /*:C CCDDataWriter
   *:D Convenience class for writing the data in a CCDData object.
       Instead of writing:
         CCDData::CheckoutKey *key = ccddata.checkout();
	 uint16_t *tenthframe = ccddata.frameData(key, 10);
	 // write to tenthframe
	 ccddata.checkin(key);
       you can write
         CCDDataWriter wrt(ccddata);
	 uint16_t *tenthframe = wrt.frameData(10);
	 // write to tenthframe
       That way, checkin is automatic when wrt goes out of scope.
  */
public:
  CCDDataWriter(CCDData &data);
  ~CCDDataWriter();
  uint16_t *frameData(int frame=-1);
  void cancel();
private:
  CCDData &data;
  CCDData::CheckoutKey *key;
  bool canceled;
};

#endif
