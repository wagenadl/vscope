// base/digitaldata.cpp - This file is part of VScope.
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

// digitaldata.cpp

#include <base/digitaldata.h>

#include <base/unitqty.h>
#include <base/dbg.h>
#include <QFile>
#include <base/progressdialog.h>

DigitalData::DigitalData(int nscans_, double fs) {
  fs_hz = fs;
  nscans = nscans_;
  data.resize(nscans);
  cmask = 0;
}

DigitalData::~DigitalData() {
}

void DigitalData::setNumScans(int nscans1) {
  if (nscans1>data.size())
    throw Exception("DigitalData","Noncredible number of scans","setNumScans");
  nscans = nscans1;
}

void DigitalData::setSamplingFrequency(double f) {
  fs_hz = f;
}


bool DigitalData::reshape(int nscans1, bool free) {
  bool r=false;
  if (nscans1 > data.size() || (free && nscans1 < data.size())) {
    data.resize(nscans1);
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

void DigitalData::read(QString ifn, QDomElement elt, ProgressDialog *pd) {
  readPartial(ifn, elt, 0, 1e9, pd);
}

void DigitalData::readPartial(QString ifn, QDomElement elt,
                              quint64 startscan, double dur_ms,
                              ProgressDialog *pd) {
  if (elt.tagName()!="digital")
    elt = elt.firstChildElement("digital");
  if (elt.isNull())
    throw Exception("DigitalData", "Cannot find xml info");

  KeyGuard guard(*this);

  /* Read aux. info from xml */
  fs_hz = UnitQty(elt.attribute("rate")).toDouble("Hz");
  if (elt.attribute("type")!="uint32")
    throw Exception("DigitalData","Cannot read type '" + elt.attribute("type")
		    + "'. (Only uint32.)");
  quint64 scans = elt.attribute("scans").toInt();
  scans -= startscan;
  if (scans > dur_ms*fs_hz/1e3)
    scans = dur_ms*fs_hz/1e3;
  reshape(scans);

  readUInt32Partial(ifn, startscan, scans, pd);

  clearMask();
  for (QDomElement e = elt.firstChildElement("line");
       !e.isNull(); e = e.nextSiblingElement("line")) {
    int idx = e.attribute("idx").toInt();
    QString id = e.attribute("id");
    defineLine(idx, id);
  }  
}

void DigitalData::writeUInt32(QString ofn) {
  QFile ofd(ofn);
  if (!ofd.open(QFile::WriteOnly))
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
  int nbytes = nscans*4;
  if (ofd.write((char const *)data.constData(), nbytes) != nbytes)
    throw SysExc("DigitalData::writeUInt32: Cannot write '" + ofn + "'");
  if (!ofd.flush())
    throw SysExc("DigitalData::writeUInt32: Cannot flush '" + ofn + "'");
  ofd.close();
}

void DigitalData::readUInt32(QString ifn, ProgressDialog *pd) {
  readUInt32Partial(ifn, 0, 1000*1000*1000, pd);
}

void DigitalData::readUInt32Partial(QString ifn,
                                    quint64 startscan, quint64 scans,
                                    ProgressDialog *pd) {
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

  ifd.seek(startscan*4);
  newscans -= startscan;
  if (newscans > scans)
    newscans = scans;
  filelength_bytes = 4*newscans;
  
  reshape(newscans);
  int offset = 0;
  while (offset<filelength_bytes) {
    int now = filelength_bytes - offset;
    if (now>65536)
      now = 65536;
    if (ifd.read((char*)data.data()+offset, now) != now)
      throw Exception("DigitalData", "Cannot read '" + ifn + "'");
    offset += now;
    if (pd)
      pd->progress(offset*100.0/filelength_bytes);
  }
  ifd.close();
  cmask = ~0;
  ensureIDs();
}

void DigitalData::clearMask() {
  setMask(0);
}

void DigitalData::ensureIDs() {
  DataType one = 1;
  for (int n=0; n<32; n++) {
    DataType msk = one<<n;
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

void DigitalData::reportLines() const {
  Dbg() << "Digital Lines:";
  for (int i=0; i<32; i++) {
    if (line2id.contains(i))
      Dbg() << i << ": " << line2id[i];
  }
}

bool DigitalData::hasLine(QString id) const {
  return id2line.contains(id);
}

QString DigitalData::lineID(unsigned int line) const {
  return hasLine(line) ? line2id[line] : "";
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
  KeyGuard guard(*this);
  data.fill(0);
}
