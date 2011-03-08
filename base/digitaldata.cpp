// digitaldata.cpp

#include <base/digitaldata.h>

#include <base/memalloc.h>
#include <base/unitqty.h>

#include <QFile>

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

  writeUInt32(ofn);
  
  elt.setAttribute("rate", UnitQty(fs_hz,"Hz").pretty(6));
  elt.setAttribute("type","uint32");
  elt.setAttribute("typebytes","4");
  elt.setAttribute("scans", QString::number(nscans));
  foreach (unsigned int n, line2id.keys()) {
    QDomElement line = elt.ownerDocument().createElement("line");
    elt.appendChild(line);
    line.setAttribute("idx", QString::number(n));
    line.setAttribute("id", line2id[n]);
  }
}

void DigitalData::read(QString ifn, QDomElement elt) {
  if (elt.tagName()!="digital")
    elt = elt.firstChildElement("digital");
  if (elt.isNull())
    throw Exception("DigitalData", "Cannot find xml info");

  KeyGuard guard(*this);
  readUInt32(ifn);

  /* Read aux. info from xml and use it! */
  fs_hz = UnitQty(elt.attribute("rate")).toDouble("Hz");
  if (elt.attribute("type")!="uint32")
    throw Exception("DigitalData","Cannot read type '" + elt.attribute("type")
		    + "'. (Only uint32.)");
  if (elt.attribute("scans").toInt() != nscans)
    throw Exception("DigitalData", "Scan count mismatch between data and xml");

  clearMask();
  for (QDomElement line = elt.firstChildElement("line");
       !elt.isNull(); elt = elt.nextSiblingElement("line")) {
    int idx = elt.attribute("idx").toInt();
    QString id = elt.attribute("id");
    defineLine(idx, id);
  }  
}

void DigitalData::writeUInt32(QString ofn) {
  QFile ofd(ofn);
  if (!ofd.open(QFile::WriteOnly))
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
  int nbytes = nscans*4;
  if (ofd.write((char const *)data, nbytes) != nbytes)
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
  if (!ofd.flush())
    throw SysExc("DigitalData::writeUInt32: Cannot flush '" + ofn + "'");
  ofd.close();
}

void DigitalData::readUInt32(QString ifn) {
  KeyGuard guard(*this);
  QFile ifd(ifn);
  if (!ifd.open(QFile::ReadOnly))
    throw Exception("DigitalData", "Cannot open '" + ifn + "'");
  int filelength_bytes = ifd.size();
  int newscans = filelength_bytes/4;
  if (newscans*4 != filelength_bytes) 
    throw Exception("DigitalData",
		       "Unexpected file size: not a multiple of scan size",
		       "readUInt32");
  reshape(newscans);
  if (ifd.read((char*)data, filelength_bytes) != filelength_bytes)
    throw Exception("DigitalData", "Cannot read '" + ifn + "'");
  ifd.close();
  cmask = ~0;
  ensureIDs();
}

void DigitalData::clearMask() {
  setMask(0);
}

void DigitalData::ensureIDs() {
  DataType msk = 1;
  for (int n=0; n<32; n++) {
    if (cmask & msk) {
      if (!line2id.contains(n)) {
	QString id = "D" + QString::number(n);
	line2id[n] = id;
	id2line[id] = n;
      }
    } else {
      if (line2id.contains(n)) {
	QString id = line2id[n];
	line2id.remove(n);
	id2line.remove(id);
      }
    }
  }
}

void DigitalData::setMask(DataType mask) {
  KeyGuard guard(*this);
  cmask = mask;
  ensureIDs();
}

void DigitalData::addLine(unsigned int line) {
  KeyGuard guard(*this);
  DataType one = 1;
  cmask |= one<<line;
  ensureIDs();
}

void DigitalData::defineLine(unsigned int line, QString id) {
  KeyGuard guard(*this);
  addLine(line);
  id2line.remove(line2id[line]);
  line2id[line] = id;
  id2line[id] = line;
}

bool DigitalData::hasLine(QString id) const {
  return id2line.contains(id);
}

bool DigitalData::hasLine(unsigned int line) const {
  return line2id.contains(line);
}

unsigned int DigitalData::findLine(QString id) const {
  if (id2line.contains(id))
    return id2line[id];
  else
    throw Exception("DigitalData","No line named " + id);
}

DigitalData::DataType DigitalData::maskForLine(unsigned int line) const {
  DataType one = 1;
  return one<<line;
}

DigitalData::DataType DigitalData::maskForLine(QString id) const {
  DataType one = 1;
  unsigned int line = findLine(id);
  return one<<line;
}

void DigitalData::zero() {
  if (data)
    for (int n=0; n<nscans; n++)
      data[n]=0;
}
