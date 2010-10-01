// analogdata.cpp

#include <base/analogdata.h>
#include <base/dbg.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define AD_SECRETCODE 0xadad0000

AnalogData::AnalogData(int nscans_, int nchannels_) throw(Exception) {
  nscans = nscans_;
  nchannels = nchannels_;
  try {
    ndoubles_allocated = nchannels * nscans;
    data = new double[ndoubles_allocated];
  } catch(...) {
    throw MemExc("AnalogData");
  }
  for (int n=0; n<nchannels; n++)
    index2channel[n]=n;
  for (int n=0; n<MAXCHANNELS; n++)
    channel2index[n]=(n<nchannels) ? n : 0;
}

AnalogData::~AnalogData() {
  try {
    if (data)
      delete [] data;
  } catch (...) {
    fprintf(stderr,"AnalogData: Memory freeing failed. Armageddon imminent.");
  }
}

/* Analog data chunk format:
   +0 AD_SECRETCODE
   +4 number of scans
   +8 number of channels
   +12 channel index
   +n interleaved data
   +k end

   Here, n is 12+4*(number of channels),
   and k is 12+4*(number of channels)+4*(number of channels)*(number of scans).

   This may be embedded in a larger file; if so the container will want to keep
   track of chunk lengths.
*/

AnalogData::AnalogData(FILE *in) throw(Exception) {
  nscans=0;
  nchannels=0;
  ndoubles_allocated=0;
  data=0;
  read(in);
}

void AnalogData::read(FILE *fd) throw(Exception) {
  uint32_t code;
  int32_t newscans;
  int32_t newchannels;
  if (fread(&code,4,1,fd)!=1)
    throw SysExc("AnalogData","File read error");
  if (code != AD_SECRETCODE)
    throw Exception("AnalogData","Inappropriate header marker");

  if (fread(&newscans,4,1,fd)!=1)
    throw SysExc("AnalogData", "File read error");
  if (fread(&newchannels,4,1,fd)!=1)
    throw SysExc("AnalogData", "File read error");
  if (newscans<0 || newchannels<0 || newchannels>MAXCHANNELS)
    throw Exception("AnalogData", "Inappropriate scan/channel count");
  if (int32_t(fread(index2channel,4,newchannels,fd))!=newchannels)
    throw SysExc("AnalogData", "File read error");
  
  for (int n=0; n<MAXCHANNELS; n++)
    channel2index[n] = 0;
  for (int n=0; n<newchannels; n++)
    channel2index[index2channel[n]] = n;
  
  if (newscans!=nscans || newchannels!=nchannels)
    reshape(newscans,newchannels);
  if (int32_t(fread(data,8,nscans*nchannels,fd)) != nscans*nchannels)
    throw SysExc("AnalogData", "File read error");
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
    try {
      if (data)
	delete [] data;
    } catch(...) {
      throw MemFreeExc("AnalogData");
    }
    try {
      data = new double[needed];
    } catch(...) {
      throw MemExc("AnalogData");
    }
    ndoubles_allocated = needed;
    r=true;
  }
  nscans = nscans1;
  nchannels = nchannels1;

  for (int n=nchannels; n<MAXCHANNELS; n++) 
    index2channel[n]=n; // this should never be actually used
  for (int n=0; n<MAXCHANNELS; n++) 
    if (channel2index[n]>=nchannels)
      channel2index[n] = 0; // effectively disconnect
  
  return r;
}


int AnalogData::write(FILE *fd) throw(Exception) {
  uint32_t code = AD_SECRETCODE;
  if (fwrite(&code,4,1,fd) != 1)
    throw SysExc("AnalogData", "File write error");
  if (fwrite(&nscans,4,1,fd) != 1)
    throw SysExc("AnalogData", "File write error");
  if (fwrite(&nchannels,4,1,fd) != 1)
    throw SysExc("AnalogData", "File write error");
  if (int32_t(fwrite(index2channel,4,nchannels,fd)) != nchannels)
    throw SysExc("AnalogData", "File write error");
  if (int32_t(fwrite(data,8,nchannels*nscans,fd)) != nchannels*nscans)
    throw SysExc("AnalogData", "File write error");
  return 12 + 4*nchannels + 8*nchannels*nscans;
}

void AnalogData::defineChannel(int index, int channel) throw(Exception) {
  if (index<0 || index>=nchannels || channel<0 || channel>=MAXCHANNELS)
    throw Exception("AnalogData", "Inappropriate channel definition");
  index2channel[index] = channel;
  channel2index[channel] = index;
}

void AnalogData::defineChannels(uint32_t channelMask) throw(Exception) {
  int count=0;
  for (int n=0; n<MAXCHANNELS; n++) {
    if (channelMask & (1<<n)) {
      index2channel[count] = n;
      channel2index[n] = count;
      ++count;
    }
  }
  if (count!=nchannels)
    throw Exception("AnalogData", "Channel definition mask has incorrect channel count");
}  

QMap<int,double> AnalogData::writeInt16(QString ofn) throw(Exception) {
  dbg("adata:writeint16. ofn=%s",qPrintable(ofn));
  dbg("  nch=%i nsc=%i",nchannels,nscans);
  double *range = new double[nchannels];
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
  for (int c=0; c<nchannels; c++)
    dbg("    range[%i] = %g",c,range[c]);
  
  int16_t *buffer = new int16_t[nchannels*1024];
  
  FILE *ofd = fopen(qPrintable(ofn),"wb");
  if (!ofd) {
    delete buffer;
    delete range;
    throw SysExc("AnalogData::writeInt16: Cannot write '" + ofn + "'");
  }

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
    if (int(fwrite(buffer, nchannels*2, now, ofd)) != now) {
      delete buffer;
      delete range;
      throw SysExc("AnalogData::writeInt16: Cannot write '" + ofn + "'");
    }
      
    scansleft -= now;
  }
  if (fclose(ofd)) {
    delete buffer;
    delete range;
    throw SysExc("AnalogData::writeInt16: Cannot write '" + ofn + "'");
  }
  dbg("    building steps");
  QMap<int,double> steps;
  for (int c=0; c<nchannels; c++)
    steps[index2channel[c]] = range[c]/32767;

  delete buffer;
  delete range;
  
  return steps;
}

void AnalogData::readInt16(QString ifn, QMap<int,double> steps)
  throw(Exception) {
  for (int c=0; c<nchannels; c++)
    if (!steps.contains(index2channel[c]))
      throw Exception("AnalogData",
			 "Stepsize not specified for some channels",
			 "readInt16");
  
  struct stat s;
  if (stat(qPrintable(ifn),&s))
    throw SysExc("AnalogData::readInt16: Cannot stat '" + ifn + "'");
  int filelength_bytes = s.st_size;
  int newscans = filelength_bytes/2/nchannels;
  if (newscans*nchannels*2 != filelength_bytes)
    throw Exception("AnalogData",
		       "Unexpected file size: not a multiple of scan size",
		       "readInt16");
  reshape(newscans,nchannels);

  FILE *ifd = fopen(qPrintable(ifn),"rb");
  if (!ifd)
    throw SysExc("AnalogData::readInt16: Cannot open '" + ifn + "'");

  int16_t *buffer = new int16_t[nchannels*1024];
  int scansleft = newscans;
  double *dp = data;
  dbg("analogdata::readint16 nchannels=%i newscans=%i",nchannels,newscans);
  while (scansleft) {
    int now = scansleft;
    if (now>1024)
      now=1024;
    int nread = fread(buffer,nchannels*2,now,ifd);
    // dbg("  now=%i nread=%i",now,nread);
    if (nread!=now) {
      delete [] buffer;
      throw SysExc("AnalogData::readInt16: Cannot read '" + ifn + "'");
    }
    int16_t *bp = buffer;
    for (int s=0; s<now; s++)
      for (int c=0; c<nchannels; c++)
	*dp++ = *bp++ * steps[index2channel[c]];
    scansleft-=now;
  }
  fclose(ifd);
  delete [] buffer;
}

bool AnalogData::contains(int ch) const {
  return (ch>=0 && ch<MAXCHANNELS)
    ? index2channel[channel2index[ch]]==ch
    : false;
}

int AnalogData::whereIsChannel(int ch) const {
  return contains(ch)
    ? channel2index[ch]
    : -1;
}

AnalogData::AnalogData(AnalogData const &other):
  AnalogData_(other) {
  copy(other);
}

void AnalogData::copy(AnalogData const &other) {
  if (data) {
    data = new double[ndoubles_allocated];
    memcpy(data, other.data, nscans*nchannels*sizeof(double));
  }
}

AnalogData &AnalogData::operator=(AnalogData const &other) {
  if (data)
    delete [] data;
  *(AnalogData*)this = other;
  copy(other);
  return *this;
}
