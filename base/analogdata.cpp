// analogdata.cpp

#include <base/analogdata.h>
#include <base/dbg.h>
#include <base/memalloc.h>
#include <base/ptrguard.h>
#include <base/unitqty.h>

AnalogData::AnalogData(int nscans_, int nchannels_, double fs_hz_) {
  nscans = nscans_;
  nchannels = nchannels_;
  fs_hz = fs_hz_;
  ndoubles_allocated = nchannels * nscans;
  data = memalloc<double>(ndoubles_allocated, "AnalogData constructor");

  index2id.resize(nchannels);
  index2scale.resize(nchannels);
  index2unit.resize(nchannels);
  id2index.clear();
  for (int n=0; n<nchannels; n++) {
    QString id ="A" + QString::number(n);
    index2id[n] = id;
    index2scale[n] = 1;
    index2unit[n] = "V";
    id2index[id] = n;
  }
}

AnalogData::~AnalogData() {
  try {
    if (data)
      delete [] data;
  } catch (...) {
    fprintf(stderr,"AnalogData: Memory freeing failed. Armageddon imminent.");
  }
}

void AnalogData::setNumScans(int nscans1) {
  KeyGuard guard(*this);
  if (nscans1*nchannels>ndoubles_allocated)
    throw Exception("AnalogData","Noncredible number of scans","setNumScans");
  nscans = nscans1;
}		   

bool AnalogData::reshape(int nscans1, int nchannels1, bool free) {
  KeyGuard guard(*this);
  int needed=nscans1*nchannels1;
  bool r=false;
  if (!data || needed > ndoubles_allocated ||
      (free && (needed < ndoubles_allocated))) {
    if (data)
      delete [] data;
    data = memalloc<double>(needed, "AnalogData::reshape");
    ndoubles_allocated = needed;
    r=true;
  }
  nscans = nscans1;
  nchannels = nchannels1;

  index2id.resize(nchannels);
  index2unit.resize(nchannels);
  index2scale.resize(nchannels);
  // add names for new channels
  for (int n=nchannels1; n<nchannels; n++) {
    QString id = "A" + QString::number(n);
    index2id[n] = id;
    index2scale[n] = 1;
    index2unit[n] = "V";
    id2index[id] = n;
  }
  // drop names for old channels
  foreach (QString c, id2index.keys())
    if (id2index[c]>=nchannels)
      id2index.remove(c);
  
  emitUnlessCheckedOut();
  return r;
}

void AnalogData::defineChannel(int index, QString id,
			       double scale, QString unit) {
  KeyGuard guard(*this);
  if (index<0 || index>=nchannels)
    throw Exception("AnalogData", "Inappropriate channel definition");

  id2index.remove(index2id[index]); // drop old definition
  
  index2id[index] = id;
  index2scale[index] = scale;
  index2unit[index] = unit;
  id2index[id] = index;
  
  emitUnlessCheckedOut();
}

void AnalogData::write(QString ofn, QDomElement elt) {
  if (elt.tagName()!="analog") {
    QDomElement e = elt.ownerDocument().createElement("analog");
    elt.appendChild(e);
    elt = e;
  }

  ScaleMap scales = writeInt16(ofn);

  elt.setAttribute("channels", QString::number(nchannels));
  elt.setAttribute("type","int16");
  elt.setAttribute("typebytes","2");
  elt.setAttribute("scans", QString::number(nscans));
  elt.setAttribute("rate", UnitQty(fs_hz,"Hz").pretty(6));
  for (int n=0; n<nchannels; n++) {
    QDomElement channel = elt.ownerDocument().createElement("channel");
    elt.appendChild(channel);
    channel.setAttribute("idx", QString::number(n));
    channel.setAttribute("id", index2id[n]);
    UnitQty scl(scales[index2id[n]]*index2scale[n],
		index2unit[n]);
    channel.setAttribute("scale", scl.pretty(6));
  }
}

AnalogData::ScaleMap AnalogData::writeInt16(QString ofn) {
  dbg("adata:writeint16. ofn=%s",qPrintable(ofn));
  dbg("  nch=%i nsc=%i",nchannels,nscans);
  PtrGuard<double> range(memalloc<double>(nchannels,
					  "AnalogData::writeInt16"));
  for (int c=0; c<nchannels; ++c)
    range[c]=1e-6; // base range is 1 uV, we don't want to get division by zero
  double *dp = data;
  for (int s=0; s<nscans; s++) {
    for (int c=0; c<nchannels; c++) {
      double v = *dp++;
      if (v>range[c])
	range[c]=v;
      else if (-v>range[c])
	range[c]=-v;
    }
  }

  PtrGuard<int16_t> buffer(memalloc<int16_t>(nchannels*1024,
					     "AnalogData::writeInt16"));
  QFile ofd(ofn);
  if (!ofd.open(QFile::WriteOnly)) 
    throw SysExc("AnalogData::writeInt16: Cannot write '" + ofn + "'");

  int scansleft = nscans;
  dp = data;
  while (scansleft>0) {
    int now = scansleft;
    if (now>1024)
      now=1024;
    int16_t *bp = buffer;
    for (int s=0; s<now; s++) 
      for (int c=0; c<nchannels; c++) 
	*bp++ = int16_t(*dp++ * 32767 / range[c]);
    int nbytes = nchannels*2*now;
    if (ofd.write((char const *)buffer.ptr(), nbytes) != nbytes)
      throw SysExc("AnalogData::writeInt16: Cannot write '" + ofn + "'");
    scansleft -= now;
  }
  ofd.close();

  ScaleMap steps;
  for (int c=0; c<nchannels; c++)
    steps[index2id[c]] = range[c]/32767;
  return steps;
}

void AnalogData::read(QString ifn, QDomElement elt) {
  KeyGuard guard(*this);

  if (elt.tagName()!="analog")
    elt = elt.firstChildElement("analog");
  if (elt.isNull())
    throw Exception("AnalogData", "Cannot find xml info");
  
  bool ok;
  fs_hz = elt.attribute("rate").toDouble(&ok);
  if (!ok)
    throw Exception("AnalogData", "Cannot read sampling rate from xml");
  int scans = elt.attribute("scans").toInt(&ok);
  if (!ok)
    throw Exception("AnalogData","Cannot read number of scans from xml");
  int chans = elt.attribute("chans").toInt(&ok);
  if (!ok)
    throw Exception("Trial","Cannot read number of channels from xml",
		    "read");
  QString dtyp = elt.attribute("type");
  if (dtyp != "int16")
    throw Exception("Trial",
		    "Only know how read analog data of type 'int16', not '"
		    + dtyp + "'", "read");
  reshape(scans, chans);
  index2id.resize(chans);
  index2scale.clear();
  index2unit.clear();
  id2index.clear();
  ScaleMap scales;
  for (QDomElement e=elt.firstChildElement("channel");
       !e.isNull(); e=e.nextSiblingElement("channel")) {
    int idx = e.attribute("idx").toInt(&ok);
    if (!ok)
      throw Exception("Trial","Cannot read channel index from xml","read");
    QString id = e.attribute("id");
    UnitQty scl(e.attribute("scale"));
    index2id[idx] = id;
    id2index[id] = idx;
    index2unit[idx] = scl.unitEPhys();
    index2scale[idx] = 1;
    scales[id] = scl.toDouble(index2unit[idx]);
  }
  readInt16(ifn, scales);
  if (nscans != scans)
    throw Exception("AnalogData", "Scan count mismatch between data and xml");
}

void AnalogData::readInt16(QString ifn, AnalogData::ScaleMap const &steps) {
  KeyGuard guard(*this);

  for (int c=0; c<nchannels; c++)
    if (!steps.contains(index2id[c]))
      throw Exception("AnalogData",
		      "Stepsize not specified for some channels",
		      "readInt16");

  QFile ifd(ifn);
  if (!ifd.open(QFile::ReadOnly)) 
    throw SysExc("AnalogData::readInt16: Cannot open '" + ifn + "'");
  int filelength_bytes = ifd.size();
  int newscans = filelength_bytes/2/nchannels;
  if (newscans*nchannels*2 != filelength_bytes)
    throw Exception("AnalogData",
		       "Unexpected file size: not a multiple of scan size",
		       "readInt16");
  reshape(newscans,nchannels);

  PtrGuard<int16_t> buffer(memalloc<int16_t>(nchannels*1024,
					     "AnalogData::readInt16"));
  int scansleft = newscans;
  double *dp = data;
  dbg("analogdata::readint16 nchannels=%i newscans=%i",nchannels,newscans);
  while (scansleft) {
    int now = scansleft;
    if (now>1024)
      now=1024;
    int nbytes = nchannels*2*now;
    if (ifd.read((char *)buffer.ptr(), nbytes) != nbytes)
      throw SysExc("AnalogData::readInt16: Cannot read '" + ifn + "'");

    int16_t *bp = buffer;
    for (int s=0; s<now; s++)
      for (int c=0; c<nchannels; c++)
	*dp++ = *bp++ * steps[index2id[c]];
    scansleft-=now;
  }
  emitUnlessCheckedOut();
}

bool AnalogData::contains(QString ch) const {
  return id2index.contains(ch);
}

int AnalogData::whereIsChannel(QString ch) const {
  return contains(ch)
    ? id2index[ch]
    : -1;
}

void AnalogData::setSamplingFrequency(double f) {
  KeyGuard guard(*this);
  fs_hz = f;
}

double const *AnalogData::channelData(QString id) const {
  if (contains(id)) 
    return data + id2index[id];
  else
    return data;
}

double *AnalogData::channelData(AnalogData::WriteKey *key, QString id) {
  verifyKey(key, "channelData");
  if (contains(id)) 
    return data + id2index[id];
  else
    return data;
}

QString AnalogData::getChannelAtIndex(int n) const {
  if (n>=0 && n<nchannels)
    return index2id[n];
  else
    return "?";
}

QString AnalogData::getUnitAtIndex(int n) const {
  if (n>=0 && n<nchannels)
    return index2unit[n];
  else
    return "?";
}

double AnalogData::getScaleAtIndex(int n) const {
  if (n>=0 && n<nchannels)
    return index2scale[n];
  else
    return 1;
}
    
