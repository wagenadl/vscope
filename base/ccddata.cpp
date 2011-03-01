// ccddata.cpp

#include "ccddata.h"
#include <base/exception.h>
#include <base/memalloc.h>
CCDData::CCDData(int serpix, int parpix, int nframes):
  serpix(serpix),
  parpix(parpix),
  nframes(nframes) {
  framepix = serpix*parpix;
  allocpix = framepix*nframes;
  data = 0;
  data = memalloc<uint16_t>(allocpix, "CCDData:: constructor");
  t0_ms = 0;
  dt_ms = 0; // default value: meaningless on purpose
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
  return realloc;
}

CCDData::~CCDData() {
  if (data)
    delete [] data;
}

uint16_t *CCDData::frameData(int frame) {
  if (frame<0)
    return data;
  if (frame>=nframes)
    throw Exception("CCDData","Bad frame number","frameData");
  return data + frame*framepix;
}

uint16_t const *CCDData::frameData(int frame) const {
  if (frame<0)
    return data;
  if (frame>=nframes)
    throw Exception("CCDData","Bad frame number","frameData");
  return data + frame*framepix;
}

void CCDData::setTimeBase(double t0, double dt) {
  t0_ms = t0;
  dt_ms = dt;
}

CCDData::CCDData(CCDData const &other): CCDData_(other) {
  data = 0;
  copy(other);
}

void CCDData::copy(CCDData const &other) {
  if (other.data) {
    data = memalloc<uint16_t>(allocpix, "CCDData");
    memcpy(data, other.data, allocpix*sizeof(uint16_t));
  }
}

CCDData &CCDData::operator=(CCDData const &other) {
  if (data)
    delete [] data;
  *(CCDData_*)this = other;
  data = 0;
  copy(other);
  return *this;
}
