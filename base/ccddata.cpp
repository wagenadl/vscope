// ccddata.cpp

#include "ccddata.h"
#include <base/exception.h>
#include <base/memalloc.h>
#include <base/dbg.h>
#include <base/unitqty.h>

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

QDomElement CCDData::write(QFile &f, QDomElement dst) const {
  if (dst.tagName()!="camera") {
    QDomElement e = dst.ownerDocument().createElement("camera");
    dst.appendChild(e);
    dst = e;
  }
  dst.setAttribute("type","uint16");
  dst.setAttribute("typebytes","2");
  dst.setAttribute("frames",QString::number(nframes));
  dst.setAttribute("serpix",QString::number(serpix));
  dst.setAttribute("parpix",QString::number(parpix));
  dst.setAttribute("delay",UnitQty(t0_ms,"ms").pretty());
  dst.setAttribute("rate",UnitQty(1/dt_ms,"kHz").pretty());
  t.write(dst);

  int nbytes = nframes*serpix*parpix*2;
  if (f.write((char const*)data, nbytes) != nbytes)
    throw Exception("CCDData","Cannot write to file");
  
  return dst;
}

void CCDData::read(QFile &f, QDomElement src) {
  if (src.tagName()!="camera")
    throw Exception("CCDData","Cannot read without a <camera> element");

  KeyGuard guard(*this);
  
  bool ok;
  int nfrm = src.attribute("frames").toInt(&ok);
  if (!ok)
    throw Exception("CCDData","Cannot read number of frames from xml","read");
  int nser = src.attribute("serpix").toInt(&ok);
  if (!ok)
    throw Exception("CCDData",
		    "Cannot read number of serial pixels from xml","read");
  int npar = src.attribute("parpix").toInt(&ok);
  if (!ok)
    throw Exception("CCDData",
		    "Cannot read number of parallel pixels from xml","read");
  if (src.attribute("type")!="uint16")
    throw Exception("CCDData",
		    "Only know how to read ccd data of type 'uint16', not '"
		    + src.attribute("type") + "'", "read");

  double t0_ms = UnitQty::str2num(src.attribute("delay"),"ms");
  double rate_hz = UnitQty::str2num(src.attribute("rate"),"Hz");
  
  reshape(nser, npar, nfrm);
  setTimeBase(t0_ms, rate_hz ? 1e3/rate_hz : 1);
  Transform t;
  if (src.hasAttribute("flipx")) {
    /* Old style format with flipx and flipy instead of full transform. */
    if (src.attribute("flipx")=="yes")
      t.flipx(nser);
    if (src.attribute("flipy")=="yes")
      t.flipy(npar);
  } else {
    t.read(src);
  }
  setDataToCanvas(t);
  
  int nbytes = nfrm*nser*npar*2;
  if (f.read((char*)data, nbytes) != nbytes)
    throw Exception("CCDData", "Cannot read CCD data");
}

