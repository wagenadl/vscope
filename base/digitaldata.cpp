// digitaldata.cpp

#include <base/digitaldata.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <base/memalloc.h>

DigitalData::DigitalData(int nscans_) throw(Exception) {
  nallocated = nscans = nscans_;
  data = 0;
  data = memalloc<uint32_t>(nscans, "DigitalData constructor");
}

DigitalData::~DigitalData() {
  try {
    delete [] data;
  } catch (...) {
    fprintf(stderr,"DigitalData: Memory freeing failed. Armageddon imminent.");
  }
}

/* Digital data chunk format:
   +0 DD_SECRETCODE
   +4 number of scans
   +8 data
   +k end

   Here k is 8+4*number of scans.

   This may be embedded in a larger file; if so the container will want to keep
   track of chunk lengths.
*/

void DigitalData::setNumScans(int nscans1) {
  if (nscans1>nallocated)
    throw Exception("DigitalData","Noncredible number of scans","setNumScans");
  nscans = nscans1;
}		   

bool DigitalData::reshape(int nscans1, bool free) {
  bool r=false;
  if (nscans1 > nallocated ||
      (free && (nscans1 < nallocated))) {
    if (data)
      delete [] data;
    data = memalloc<uint32_t>(nscans1, "AnalogData");
    nallocated = nscans1;
    r=true;
  }
  nscans = nscans1;
  return r;
}

void DigitalData::writeUInt32(QString ofn) throw(Exception) {
  FILE *ofd = fopen(qPrintable(ofn),"wb");
  if (!ofd)
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");

  if (int32_t(fwrite(data,4,nscans,ofd))!=nscans) {
    fclose(ofd);
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
  }

  if (fclose(ofd))
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
}

void DigitalData::readUInt32(QString ifn) throw(Exception) {
  struct stat s;
  if (stat(qPrintable(ifn),&s))
    throw SysExc("DigitalData::readUInt32: Cannot stat '" + ifn + "'");
  int filelength_bytes = s.st_size;
  int newscans = filelength_bytes/4;
  if (newscans*4 != filelength_bytes) 
    throw Exception("DigitalData",
		       "Unexpected file size: not a multiple of scan size",
		       "readUInt32");
  reshape(newscans);

  FILE *ifd = fopen(qPrintable(ifn),"rb");
  if (!ifd)
    throw SysExc("DigitalData::readUInt32: Cannot read '" + ifn + "'");

  if (int32_t(fread(data,4,newscans,ifd)) != newscans) {
    fclose(ifd);
    throw SysExc("DigitalData::readUInt32: Cannot read '" + ifn + "'");
  }
  cmask = ~0;
  fclose(ifd);
}

void DigitalData::clearMask() {
  cmask=0;
}

void DigitalData::addLine(uint32_t line) {
  uint32_t one = 1;
  cmask |= one<<line;
}

bool DigitalData::hasLine(uint32_t line) const {
  uint32_t one = 1;
  uint32_t msk = one<<line;
  return (cmask & msk) != 0;
}

DigitalData::DigitalData(DigitalData const &other):
  DigitalData_(other) {
  data = 0;
  if (other.data) {
    reshape(other.nscans);
    memcpy(data, other.data, nscans*sizeof(uint32_t));
  }
}


DigitalData &DigitalData::operator=(DigitalData const &other) {
  if (data)
    delete [] data;
  *(DigitalData_*)this = other;
  data = 0;
  if (other.data) {
    reshape(other.nscans);
    memcpy(data, other.data, nscans*sizeof(uint32_t));
  }
  return *this;
}

void DigitalData::zero() {
  if (data)
    for (int n=0; n<nscans; n++)
      data[n]=0;
}
