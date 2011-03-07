// digitaldata.cpp

#include <base/digitaldata.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <base/memalloc.h>

DigitalData::DigitalData(int nscans_, double fs) {
  fs_hz = fs;
  nallocated = nscans = nscans_;
  data = 0;
  data = memalloc<DataType>(nscans, "DigitalData constructor");
  cmask = 0;
}

DigitalData::~DigitalData() {
  try {
    delete [] data;
  } catch (...) {
    fprintf(stderr,"DigitalData: Memory freeing failed. Armageddon imminent.");
  }
}

void DigitalData::setNumScans(int nscans1) {
  if (nscans1>nallocated)
    throw Exception("DigitalData","Noncredible number of scans","setNumScans");
  nscans = nscans1;
}

void DigitalData::setSamplingFrequency(double f) {
  fs_hz = f;
}


bool DigitalData::reshape(int nscans1, bool free) {
  bool r=false;
  if (nscans1 > nallocated ||
      (free && (nscans1 < nallocated))) {
    if (data)
      delete [] data;
    data = memalloc<DataType>(nscans1, "AnalogData");
    nallocated = nscans1;
    r=true;
  }
  nscans = nscans1;
  return r;
}

void DigitalData::write(QString ofn, QDomElement elt) {
  if (elt.tagName()!="digital") {
    QDomElement e = elt.ownerDocument().createElement("digital");
    elt.appendChild(e);
    elt = e;
  }
  elt.setAttribute("rate", UnitQty(fs_hz,"Hz").pretty(6));
  elt.setAttribute("type","uint32");
  elt.setAttribute("typebytes","4");
  elt.setAttribute("scans", QString::number(nscans));
  foreach (unsigned int n, line2id.keys()) {
    QDomElement line = elt.ownerDocument().createElement("line");
    elt.appendChild(line);
    channel.setAttribute("idx", QString::number(n));
    channel.setAttribute("id", line2id[n]);
  }
}

void DigitalData::writeUInt32(QString ofn) {
  QFile ofd(ofn);
  if (!ofd.open(QFile::WriteOnly))
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
  int nbytes = nscans*4;
  if (ofd.write((char const *)data, nbytes) != nbytes)
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
  ofd.close();
}

void DigitalData::read(QString ifn, QDomElement elt) {
  if (elt.tagName()!="digital")
    elt = elt.firstChildElement("digital");
  if (elt.isNull())
    throw Exception("DigitalData", "Cannot find xml info");

  KeyGuard guard(this);
  /* Read aux. info from xml and use it! */
  readUInt32(ifn);
  
}

void DigitalData::readUInt32(QString ifn) {
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
  setMask(0);
}

void DigitalData::setMask(DataType mask) {
  cmask = mask;
  ensureIDs();
}

void DigitalData::addLine(unsigned int line) {
  DataType one = 1;
  cmask |= one<<line;
  ensureIDs();
}

void DigitalData::defineLine(unsigned int line, QString id) {
  if (hasLine(line)) 
    id2line.remove(line2id[line]);
  line2id[line] = id;
  id2line[id] = line;
}

bool hasLine(QString id) const {
  return id2line.contains(id);
}

bool DigitalData::hasLine(unsigned int line) const {
  return line2id.contains(line);
}

void DigitalData::zero() {
  if (data)
    for (int n=0; n<nscans; n++)
      data[n]=0;
}
