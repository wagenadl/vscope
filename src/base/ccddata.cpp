// base/ccddata.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// ccddata.cpp

#include "ccddata.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <base/unitqty.h>
#include <base/progressdialog.h>

CCDData::CCDData(int serpix_, int parpix_, int nframes_) {
  serpix = serpix_;
  parpix = parpix_;
  nframes = nframes_;
  framepix = serpix*parpix;
  data.resize(framepix*nframes);
  t0_ms = 0;
  dt_ms = 0; // default value: meaningless on purpose
  dur_ms = 0; // default value: meaningless on purpose
}

bool CCDData::reshape(int ser, int par, int nfr, bool free) {
  int needed = ser*par*nfr;
  bool realloc = needed>data.size() || (needed<data.size() && free);
  if (realloc)
    data.resize(needed);
  serpix = ser;
  parpix = par;
  framepix = ser*par;
  nframes = nfr;
  emitUnlessCheckedOut();
  return realloc;
}

CCDData::~CCDData() {
}

uint16_t const *CCDData::frameData(int frame) const {
  if (frame<0)
    return data.constData();
  else if (frame<nframes)
    return data.constData() + frame*framepix;
  else
    throw Exception("CCDData", "Bad frame number: " + QString::number(frame),
                    "frameData");

}

uint16_t *CCDData::frameData(WriteKey *key, int frame) {
  verifyKey(key,"frameData called with unknown key");
  if (frame<0)
    return data.data();
  else if (frame<nframes)
    return data.data() + frame*framepix;
  else
    throw Exception("CCDData","Bad frame number: " + QString::number(frame),
                    "frameData");
}

void CCDData::setTimeBase(double t0, double dt, double dur) {
  t0_ms = t0;
  dt_ms = dt;
  dur_ms = dur;
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
  dst.setAttribute("framedur",UnitQty(dur_ms,"ms").pretty());
  t.write(dst);

  int nbytes = nframes*serpix*parpix*2;
  Q_ASSERT(data.size()>=serpix*parpix*nframes);
  if (f.write((char const*)data.constData(), nbytes) != nbytes)
    throw Exception("CCDData","Cannot write to file");
  
  return dst;
}

void CCDData::read(QFile &f, QDomElement src, ProgressDialog *pd) {
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
  double dur_ms = UnitQty::str2num(src.attribute("framedur"),"ms");
  
  reshape(nser, npar, nfrm);
  setTimeBase(t0_ms, rate_hz ? 1e3/rate_hz : 1, dur_ms);
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
  Q_ASSERT(data.size() >= nfrm*nser*npar);
  int offset = 0;
  while (offset<nbytes) {
    int now = nbytes - offset;
    if (now>65536)
      now = 65536;
    if (f.read((char*)data.data()+offset, now) != now)
      throw Exception("CCDData", "Cannot read CCD data");
    offset+=now;
    if (pd)
      pd->progress(offset*100.0/nbytes);
  }
}

void CCDData::zero() {
  data.fill(0);
}
