// ccddata.cpp

#include "ccddata.h"
#include <base/exception.h>

CCDData::CCDData(int serpix, int parpix, int nframes):
  serpix(serpix),
  parpix(parpix),
  nframes(nframes) {
  framepix = serpix*parpix;
  allocpix = framepix*nframes;
  try {
    data = new uint16_t[allocpix];
  } catch (...) {
    throw MemExc("CCDData::constructor");
  }
  if (data==0)
    throw MemExc("CCDData::constructor: 0 from new");
}

bool CCDData::reshape(int ser, int par, int nfr, bool free) {
  int needed = ser*par*nfr;
  bool realloc = needed>allocpix || (needed<allocpix && free);
  if (realloc) {
    delete [] data;
    data=0;
    try {
      allocpix = needed;
      data = new uint16_t[allocpix];
    } catch(...) {
      throw MemExc("CCDData::reshape");
    }
    if (data==0)
      throw MemExc("CCDData::reshape: 0 from new");
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
  if (frame<0 || frame>=nframes)
    throw Exception("CCDData","Bad frame number","frameData");
  return data + frame*framepix;
}

uint16_t const *CCDData::frameData(int frame) const {
  if (frame<0 || frame>=nframes)
    throw Exception("CCDData","Bad frame number","frameData");
  return data + frame*framepix;
}

void CCDData::setTimeBase(double t0, double dt) {
  t0_ms = t0;
  dt_ms = dt;
}

CCDData::CCDData(CCDData const &other): CCDData_(other) {
  copy(other);
}

void CCDData::copy(CCDData const &other) {
  if (data) {
    data = new uint16_t[allocpix];
    memcpy(data, other.data, allocpix*sizeof(uint16_t));
  }
}

CCDData &CCDData::operator=(CCDData const &other) {
  if (data)
    delete [] data;
  *(CCDData_*)this = other;
  copy(other);
  return *this;
}
