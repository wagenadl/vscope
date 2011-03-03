// ccddata.cpp

#include "ccddata.h"
#include <base/exception.h>
#include <base/memalloc.h>
#include <base/dbg.h>

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
  checkedout = false;
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
  if (!checkedout)
    emit newData();
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

uint16_t *CCDData::frameData(CheckoutKey, int frame) {
  if (frame<0)
    return data;
  if (frame>=nframes)
    throw Exception("CCDData","Bad frame number","frameData");
  return data + frame*framepix;
}

void CCDData::setTimeBase(double t0, double dt) {
  t0_ms = t0;
  dt_ms = dt;
  if (!checkedout)
    emit newData();
}

void CCDData::setDataToCanvas(Transform const &t0) {
  t = t0;
  if (!checkedout)
    emit newData();
}

CCDData::CheckoutKey CCDData::checkout() {
  checkedout = true;
  return CheckoutKey();
}

void CCDData::cancel(CCDData::CheckoutKey) {
  checkedout = false;
}

void CCDData::checkin(CCDData::CheckoutKey) {
  checkedout = false;
  emit newData();
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

uint16_t CCDDataWriter::frameData(int frame) {
  return data.frameData(key, frame);
}
