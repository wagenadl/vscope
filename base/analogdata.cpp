// analogdata.cpp

#include <base/analogdata.h>
#include <base/dbg.h>
#include <base/memalloc.h>
#include <base/ptrguard.h>

AnalogData::AnalogData(int nscans_, int nchannels_, double fs_hz_)
  throw(Exception) {
  nscans = nscans_;
  nchannels = nchannels_;
  fs_hz = fs_hz_;
  ndoubles_allocated = nchannels * nscans;
  data = memalloc<double>(ndoubles_allocated, "AnalogData constructor");

  index2channel.resize(nchannels);
  for (int n=0; n<nchannels; n++)
    index2channel[n] = n;

  channel2index.clear();
  for (int n=0; n<nchannels; n++)
    channel2index[n] = n;
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
  foreach (int32_t c, channel2index.keys())
    if (channel2index[c]>=nchannels)
      channel2index.remove(c);
  return r;
}

void AnalogData::defineChannel(int index, int channel) throw(Exception) {
  if (index<0 || index>=nchannels || channel<0)
    throw Exception("AnalogData", "Inappropriate channel definition");
  index2channel[index] = channel;
  foreach (int32_t c, channel2index.keys())
    if (c!=channel && channel2index[c]==index)
      channel2index.remove(c);
  channel2index[channel] = index;
}

QMap<int,double> AnalogData::writeInt16(QString ofn) throw(Exception) {
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

  QMap<int,double> steps;
  for (int c=0; c<nchannels; c++)
    steps[index2channel[c]] = range[c]/32767;
  return steps;
}

void AnalogData::readInt16(QString ifn, QMap<int,double> steps)
  throw(Exception) {
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
}

bool AnalogData::contains(int ch) const {
  return channel2index.contains(ch);
}

int AnalogData::whereIsChannel(int ch) const {
  return contains(ch)
    ? channel2index[ch]
    : -1;
}

AnalogData::AnalogData(AnalogData const &other) {
  data = 0;
  *this = other;
}

AnalogData &AnalogData::operator=(AnalogData const &other) {
  if (data)
    delete [] data;
  data = 0;
  fs_hz = other.fs_hz;
  index2channel = other.index2channel;
  channel2index = other.channel2index;
  if (other.data) {
    reshape(other.nscans, other.nchannels);
    memcpy(data, other.data, nscans*nchannels*sizeof(double));
  }
  return *this;
}

void AnalogData::setSamplingFrequency(double f) {
  fs_hz = f;
}
