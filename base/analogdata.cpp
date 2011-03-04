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

  index2channel.resize(nchannels);
  index2scale.resize(nchannels);
  index2unit.resize(nchannels);
  channel2index.clear();
  for (int n=0; n<nchannels; n++) {
    QString id ="A" + QString::number(n);
    index2channel[n] = id;
    index2scale[n] = 1;
    index2unit[n] = "V";
    channel2index[id] = n;
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
  if (nscans1*nchannels>ndoubles_allocated)
    throw Exception("AnalogData","Noncredible number of scans","setNumScans");
  nscans = nscans1;
  emitUnlessCheckedOut();
}		   

bool AnalogData::reshape(int nscans1, int nchannels1, bool free) {
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

  index2channel.resize(nchannels);
  index2unit.resize(nchannels);
  index2scale.resize(nchannels);
  // add names for new channels
  for (int n=nchannels1; n<nchannels; n++) {
    QString id = "A" + QString::number(n);
    index2channel[n] = id;
    index2scale[n] = 1;
    index2unit[n] = "V";
    channel2index[id] = n;
  }
  // drop names for old channels
  foreach (QString c, channel2index.keys())
    if (channel2index[c]>=nchannels)
      channel2index.remove(c);
  
  emitUnlessCheckedOut();
  return r;
}

void AnalogData::defineChannel(int index, QString channel,
			       double scale, QString unit) {
  if (index<0 || index>=nchannels)
    throw Exception("AnalogData", "Inappropriate channel definition");

  channel2index.remove(index2channel[index]); // drop old definition
  
  index2channel[index] = channel;
  index2scale[index] = scale;
  index2unit[index] = unit;
  channel2index[channel] = index;
  
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
    channel.setAttribute("id", index2channel[n]);
    UnitQty scl(scales[index2channel[n]]*index2scale[n],
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
    if (ofd.write((char const *)(int16_t*)buffer, nbytes) != nbytes)
      throw SysExc("AnalogData::writeInt16: Cannot write '" + ofn + "'");
    scansleft -= now;
  }
  ofd.close();

  ScaleMap steps;
  for (int c=0; c<nchannels; c++)
    steps[index2channel[c]] = range[c]/32767;
  return steps;
}

void AnalogData::read(QString ifn, QDomElement elt) {
  if (elt.tagName()!="analog")
    elt = elt.firstChildElement("analog");
  if (elt.isNull())
    throw Exception("AnalogData", "Cannot find xml info");

  KeyGuard guard(this);
  
  AnalogData::ScaleMap scales;
  // etcetera
  readInt16(ifn, scales);
}

void AnalogData::readInt16(QString ifn, AnalogData::ScaleMap const &steps) {
  for (int c=0; c<nchannels; c++)
    if (!steps.contains(index2channel[c]))
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
    if (ifd.read((char *)(int16_t*)buffer, nbytes) != nbytes)
      throw SysExc("AnalogData::readInt16: Cannot read '" + ifn + "'");

    int16_t *bp = buffer;
    for (int s=0; s<now; s++)
      for (int c=0; c<nchannels; c++)
	*dp++ = *bp++ * steps[index2channel[c]];
    scansleft-=now;
  }
  emitUnlessCheckedOut();
}

bool AnalogData::contains(QString ch) const {
  return channel2index.contains(ch);
}

int AnalogData::whereIsChannel(QString ch) const {
  return contains(ch)
    ? channel2index[ch]
    : -1;
}

void AnalogData::setSamplingFrequency(double f) {
  fs_hz = f;
  emitUnlessCheckedOut();
}

double const *AnalogData::channelData(QString channel) const {
  if (contains(channel)) 
    return data + channel2index[channel];
  else
    return data;
}

double *AnalogData::channelData(AnalogData::WriteKey *key, QString channel) {
  verifyKey(key, "channelData");
  if (contains(channel)) 
    return data + channel2index[channel];
  else
    return data;
}

QString AnalogData::getChannelAtIndex(int n) const {
  if (n>=0 && n<nchannels)
    return index2channel[n];
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
    
