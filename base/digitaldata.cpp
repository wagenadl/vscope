// digitaldata.cpp

#include <base/digitaldata.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#define DD_SECRETCODE 0xdddd0000

DigitalData::DigitalData(int nscans_) throw(Exception) {
  nallocated = nscans = nscans_;
  // cmask = 0;
  try {
    data = new uint32_t[nscans];
  } catch(...) {
    throw MemExc("DigitalData");
  }
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

DigitalData::DigitalData(FILE *in) throw(Exception) {
  nscans=0;
  data=0;
  read(in);
}

void DigitalData::read(FILE *fd) throw(Exception) {
  uint32_t code;
  int32_t newscans;
  if (fread(&code,4,1,fd)!=1)
    throw SysExc("DigitalData","File read error");
  if (code != DD_SECRETCODE)
    throw Exception("DigitalData","Inappropriate header marker");

  if (fread(&newscans,4,1,fd)!=1)
    throw SysExc("DigitalData", "File read error");
  if (newscans<0)
    throw Exception("DigitalData", "Inappropriate scan count");
  
  if (newscans!=nscans)
    reshape(newscans);

  if (fread(data,4,nscans,fd) != (unsigned int)nscans)
    throw SysExc("DigitalData", "File read error");
}


void DigitalData::setNumScans(int nscans1) {
  if (nscans1>nallocated)
    throw Exception("DigitalData","Noncredible number of scans","setNumScans");
  nscans = nscans1;
}		   

bool DigitalData::reshape(int nscans1, bool free) {
  bool r=false;
  if (nscans1 > nallocated ||
      (free && (nscans1 < nallocated))) {
    try {
      if (data)
	delete [] data;
    } catch(...) {
      throw MemFreeExc("AnalogData");
    }
    try {
      data = new uint32_t[nscans1];
    } catch(...) {
      throw MemExc("AnalogData");
    }
    nallocated = nscans1;
    r=true;
  }
  nscans = nscans1;
  return r;
}


int DigitalData::write(FILE *out) throw(Exception) {
  uint32_t code = DD_SECRETCODE;
  if (fwrite(&code,4,1,out) != 1)
    throw SysExc("DigitalData", "File header write error");
  if (fwrite(&nscans,4,1,out) != 1)
    throw SysExc("DigitalData", "File header write error");
  if (int32_t(fwrite(data,4,nscans,out)) != nscans)
    throw SysExc("DigitalData", "File data write error");
  return 8 + 4*nscans;
}

void DigitalData::writeUInt32(QString ofn) throw(Exception) {
  FILE *ofd = fopen(qPrintable(ofn),"wb");
  if (!ofd)
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");

  if (int32_t(fwrite(data,4,nscans,ofd))!=nscans)
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");

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

  if (int32_t(fread(data,4,newscans,ifd)) != newscans)
    throw SysExc("DigitalData::readUInt32: Cannot read '" + ifn + "'");

  fclose(ifd);
}

void DigitalData::clearMask() {
  cmask=0;
}

void DigitalData::addLine(uint32_t line) {
  uint32_t one = 1;
  cmask |= one<<line;
}



DigitalData::DigitalData(DigitalData const &other):
  DigitalData_(other) {
  copy(other);
}

void DigitalData::copy(DigitalData const &other) {
  if (data) {
    data = new uint32_t[nallocated];
    memcpy(data, other.data, nscans*sizeof(uint32_t));
  }
}

DigitalData &DigitalData::operator=(DigitalData const &other) {
  if (data)
    delete [] data;
  *(DigitalData*)this = other;
  copy(other);
  return *this;
}
