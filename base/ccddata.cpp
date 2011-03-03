// ccddata.cpp

#include "ccddata.h"
#include <base/exception.h>
#include <base/memalloc.h>
#include <base/dbg.h>

class CCDData::CheckoutKey {
};

CCDData::CCDData(int serpix_, int parpix_, int nframes_) {
  serpix = serpix_;
  parpix = parpix_;
  nframes = nframes_;
  framepix = serpix*parpix;
  allocpix = framepix*nframes;
  data = 0;
  data = memalloc<uint16_t>(allocpix, "CCDData:: constructor");
  t0_ms = 0;
  dt_ms = 0; // default value: meaningless on purpose
  mustEmit = false;
}

bool CCDData::reshape(int ser, int par, int nfr, bool free) {
  int needed = ser*par*nfr;
  bool realloc = needed>allocpix || (needed<allocpix && free);
  if (realloc) {
    delete [] data;
    data=0;
    allocpix = needed;
    data = memalloc<uint16_t>(allocpix, "CCDData::reshape");
  }
  serpix = ser;
  parpix = par;
  framepix = ser*par;
  nframes = nfr;
  emitUnlessCheckedOut();
  return realloc;
}

CCDData::~CCDData() {
  if (data)
    delete [] data;
}

uint16_t const *CCDData::frameData(int frame) const {
  if (frame<0)
    return data;
  if (frame>=nframes)
    throw Exception("CCDData","Bad frame number","frameData");
  return data + frame*framepix;
}

uint16_t *CCDData::frameData(CheckoutKey *, int frame) {
  if (frame<0)
    return data;
  if (frame>=nframes)
    throw Exception("CCDData","Bad frame number","frameData");
  return data + frame*framepix;
}

void CCDData::emitUnlessCheckedOut() {
  if (keys.isEmpty()) {
    mustEmit = false;
    emit newData();
  } else {
    mustEmit = true;
  }
}  

void CCDData::setTimeBase(double t0, double dt) {
  t0_ms = t0;
  dt_ms = dt;
  emitUnlessCheckedOut();
}

void CCDData::setDataToCanvas(Transform const &t0) {
  t = t0;
  emitUnlessCheckedOut();
}

CCDData::CheckoutKey *CCDData::checkout() {
  CheckoutKey *key = new CheckoutKey();
  keys.insert(key);
  return key;
}

void CCDData::cancel(CCDData::CheckoutKey *key) {
  if (!keys.contains(key))
    throw Exception("CCDData", "Canceling with unknown key");
  keys.remove(key);
  delete key;
  if (mustEmit)
    emitUnlessCheckedOut();
}

void CCDData::checkin(CCDData::CheckoutKey *key) {
  if (!keys.contains(key))
    throw Exception("CCDData", "Checking in with unknown key");
  keys.remove(key);
  delete key;
  emitUnlessCheckedOut();
}

CCDDataWriter::CCDDataWriter(CCDData &data): data(data) {
  key = data.checkout();
  canceled = false;
}

void CCDDataWriter::cancel() {
  canceled = true;
}

CCDDataWriter::~CCDDataWriter() {
  if (canceled)
    data.cancel(key);
  else
    data.checkin(key);
}

uint16_t *CCDDataWriter::frameData(int frame) {
  return data.frameData(key, frame);
}
