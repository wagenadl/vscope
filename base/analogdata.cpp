// analogdata.cpp

#include <base/analogdata.h>
#include <base/dbg.h>
#include <base/unitqty.h>

AnalogData::AnalogData(int nscans_, int nchannels_, double fs_hz_) {
  nscans = nscans_;
  nchannels = nchannels_;
  fs_hz = fs_hz_;
  data.resize(nchannels * nscans);

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
}

void AnalogData::setNumScans(int nscans1) {
  KeyGuard guard(*this);
  if (nscans1*nchannels>data.size())
    throw Exception("AnalogData","Noncredible number of scans","setNumScans");
  nscans = nscans1;
}		   

bool AnalogData::reshape(int nscans1, int nchannels1, bool free) {
  KeyGuard guard(*this);
  int needed=nscans1*nchannels1;
  bool r=false;
  if (needed > data.size() || (free && needed < data.size())) {
    data.resize(needed);
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
  Q_ASSERT(nchannels*nscans <= data.size());
  
  QVector<double> range(nchannels, 1e-6);
  // base range is 1 uV, we don't want to get division by zero

  double const *dp = data.constData();
  for (int s=0; s<nscans; s++) {
    for (int c=0; c<nchannels; c++) {
      double v = *dp++;
      if (v>range[c])
	range[c]=v;
      else if (-v>range[c])
	range[c]=-v;
    }
  }

  const int BUFSIZE = 1024;
  QVector<int16_t> buffer(nchannels*BUFSIZE);
  QFile ofd(ofn);
  if (!ofd.open(QFile::WriteOnly)) 
    throw SysExc("AnalogData::writeInt16: Cannot write '" + ofn + "'");

  int scansleft = nscans;
  dp = data.constData();
  while (scansleft>0) {
    int now = scansleft < BUFSIZE ? scansleft : BUFSIZE;
    int16_t *bp = buffer.data();
    for (int s=0; s<now; s++) 
      for (int c=0; c<nchannels; c++) 
	*bp++ = int16_t(*dp++ * 32767 / range[c]);
    int nbytes = nchannels*2*now;
    if (ofd.write((char const *)buffer.constData(), nbytes) != nbytes)
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
  UnitQty fs(elt.attribute("rate"));
  fs_hz = fs.toDouble("Hz");
  int scans = elt.attribute("scans").toInt(&ok);
  if (!ok)
    throw Exception("AnalogData","Cannot read number of scans from xml");
  int chans = elt.attribute("channels").toInt(&ok);
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
  index2scale.resize(chans);
  index2unit.resize(chans);
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

  const int BUFSIZE = 1024;
  QVector<int16_t> buffer(nchannels*BUFSIZE);
  int scansleft = newscans;
  double *dp = data.data();
  while (scansleft) {
    int now = scansleft < BUFSIZE ? scansleft : BUFSIZE;
    int nbytes = nchannels*2*now;
    if (ifd.read((char *)buffer.data(), nbytes) != nbytes)
      throw SysExc("AnalogData::readInt16: Cannot read '" + ifn + "'");

    int16_t const *bp = buffer.constData();
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

double const *AnalogData::allData() const {
  return data.constData();
}

double *AnalogData::allData(WriteKey *key) {
  verifyKey(key, "channelData");
  return data.data();
}
double const *AnalogData::channelData(QString id) const {
  if (contains(id)) 
    return data.constData() + id2index[id];
  else
    return data.constData();
}

double *AnalogData::channelData(AnalogData::WriteKey *key, QString id) {
  verifyKey(key, "channelData");
  if (contains(id)) 
    return data.data() + id2index[id];
  else
    return data.data();
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
    
