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
  else if (frame<nframes)
    return data + frame*framepix;
  else
    throw Exception("CCDData","Bad frame number","frameData");

}

uint16_t *CCDData::frameData(WriteKey *key, int frame) {
  verifyKey(key,"frameData called with unknown key");
  if (frame<0)
    return data;
  else if (frame<nframes)
    return data + frame*framepix;
  else
    throw Exception("CCDData","Bad frame number","frameData");
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
