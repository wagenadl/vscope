// daq/daqbase.cpp - This file is part of VScope.
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

// daqBase.cpp

#include <daq/daqbase.h>
#include <../nidaq/NIDAQmx.h>
#include <base/dbg.h>
#include <base/exception.h>

void daqTry(int errcode, char const *issuer,
	    char const *msg, char const *aux) /*throw(daqException)*/ {
  if (errcode==0)
    return;
  if (errcode==-1) {
    QString m = msg; m+= ": No hardware";
    daqException exc(issuer, m, aux);
    throw exc;
  }

  int n = DAQmxGetErrorString(errcode, 0, 0);
  // daqTry(DAQmxGetErrorString()) // for daqmx2dummy!
  if (n<0 || n>1000)
    n=1000;
  QByteArray ar(n, 0);
  int e = DAQmxGetErrorString(errcode, ar.data(), n);
  QString daqerr = (e<0) ? "unknown" : QString(ar);
  fprintf(stderr,"DAQ Problem #%i: '%s'\n", n, daqerr.toUtf8().constData());

  if (errcode>0) {
    // this is a warning only
    fprintf(stderr,"Warning: %s", issuer ? issuer : "");
    if (aux)
      fprintf(stderr," (%s)", aux);
    if (msg)
      fprintf(stderr,": %s", msg);
    fprintf(stderr,": %s\n", daqerr.toUtf8().constData());
  } else {
    daqException exc(issuer, msg, aux);
    exc.addMessage(daqerr);
    throw exc;
  }
}

DAQDevice::DAQDevice(QString id) {
  id_ = id;
  dioport = 0;
  ndiolines = -1;
}

DAQDevice::~DAQDevice() {
}

bool DAQDevice::ok(DAQDevice const *dd) {
  return dd && dd->isValid();
}

bool DAQDevice::isValid() const {
  return !id_.isEmpty();
}

void DAQDevice::demand() {
  if (!ok(this))
    throw daqException("DAQDevice","Device not found");
}

QString DAQDevice::id() {
  return ok(this) ? id_ : "none";
}

QString DAQDevice::serialNo() {
  return ok(this) ? devSerialNo(id_) : "none";
}

QString DAQDevice::typeName() {
  return ok(this) ? devTypeName(id_) : "none";
}

unsigned int DAQDevice::typeCode() {
  return ok(this) ? devTypeCode(id_) : 0;
}

int DAQDevice::nAIChannels() {
  return ok(this) ? devAIChannels(id_).size() : 0;
}

int DAQDevice::nAOChannels() {
  return ok(this) ? devAOChannels(id_).size() : 0;
}

void DAQDevice::setDIOPort(int p) {
  dioport = p;
  ndiolines = -1;
}

int DAQDevice::dioPort() {
  return dioport;
}

int DAQDevice::nDIOLines() {
  if (!ok(this))
    return 0;
  if (ndiolines<0) 
    ndiolines = devDILines(id_, dioport).size();
  return ndiolines;
}
  
QStringList DAQDevice::deviceList() {
  char buffer[2048];
  QStringList devs;
  if (DAQmxGetSysDevNames(buffer,2048))
    return devs;
  // daqTry(DAQmxGetSysDevNames())
  char *devptr = buffer;
  while ((devptr = strtok(devptr, ", "))) {
    devs.append(devptr);
    devptr = 0;
  }
  return devs;
}

QString DAQDevice::devTypeName(QString id) {
  char typ[256];
  daqTry(DAQmxGetDevProductType(id.toUtf8().constData(), typ, 255),
	 "DAQDevice", "Cannot get product type name");
  return typ;
}

unsigned int DAQDevice::devTypeCode(QString id) {
  uInt32 prodnum;
  daqTry(DAQmxGetDevProductNum(id.toUtf8().constData(), &prodnum),
	 "DAQDevice", "Cannot get product number");
  return prodnum;
}

QString DAQDevice::devSerialNo(QString id) {
  uInt32 serno;
  daqTry(DAQmxGetDevSerialNum(id.toUtf8().constData(), &serno),
	 "productSerNo", "Cannot read serial number");
  return QString::number(serno,16);
}

QStringList DAQDevice::devAIChannels(QString id) {
  QStringList ch;
  char buffer[2048];
  daqTry(DAQmxGetDevAIPhysicalChans(id.toUtf8().constData(),
				    buffer, 2048),
	 "DAQDevice", "Cannot read channel names");
  char *cptr = buffer;
  while ((cptr = strtok(cptr, ", "))) {
    QString c = cptr;
    c.replace(id+"/","");
    ch.append(c);
    cptr = 0;
  }
  return ch;
}
   
QStringList DAQDevice::devAOChannels(QString id) {
  QStringList ch;
  char buffer[2048];
  daqTry(DAQmxGetDevAOPhysicalChans(id.toUtf8().constData(),
				    buffer, 2048),
	 "DAQDevice", "Cannot read channel names");
  char *cptr = buffer;
  while ((cptr = strtok(cptr, ", "))) {
    QString c = cptr;
    c.replace(id+"/","");
    ch.append(c);
    cptr = 0;
  }
  return ch;
}

QStringList DAQDevice::devDILines(QString devid, int portno) {
  QStringList ch;
  char buffer[2048];
  QString portname = QString("port%1/").arg(portno);
  daqTry(DAQmxGetDevDILines(devid.toUtf8().constData(),
			    buffer, 2048),
	 "DAQDevice", "Cannot read line names");

  char *cptr = buffer;
  while ((cptr = strtok(cptr, ", "))) {
    QString c = cptr;
    c.replace(devid+"/","");
    if (c.startsWith(portname)) {
      c.replace(portname,"");
      ch.append(c);
    }
    cptr = 0;
  }

  return ch;
}

QMap<QString, int> DAQDevice::aliases;
QVector<DAQDevice *> DAQDevice::devices;

QString DAQDevice::search(QString prodtype, QString serno) {
  QString id="";
  QStringList devs = deviceList();
  for (QStringList::iterator i=devs.begin(); i!=devs.end(); ++i) { 
    if ((prodtype=="" || devTypeName(*i)==prodtype) &&
	(serno=="" || devSerialNo(*i)==serno)) {
      id = *i;
      break;
    }
  }
  return id;
}

QString DAQDevice::search(unsigned int prodtype, unsigned int sernoi) {
  QString id = "";
  QStringList devs = deviceList();
  QString serno = sernoi ? QString::number(sernoi,16) : "";
  for (QStringList::iterator i=devs.begin(); i!=devs.end(); ++i) {
    if ((prodtype==0 ||devTypeCode(*i)==prodtype) &&
	(serno=="" || devSerialNo(*i)==serno)) {
      id = *i;
      break;
    }
  }
  return id;
}

DAQDevice &DAQDevice::find(QString alias) {
  initialize();
    
  if (aliases.contains(alias)) {
    int n = aliases[alias];
    return *(devices[n]);
  }

  throw Exception("DAQDevice","Cannot find requested device");
}

void DAQDevice::addAlias(QString id, QString alias) {
  initialize();


  if (aliases.contains(id))
    aliases[alias] = aliases[id];
  else
    throw Exception("DAQDevice","Cannot find requested device");
}

void DAQDevice::initialize() {
  if (!devices.isEmpty())
    return; // already initialized

  
  QStringList ids = deviceList();
  if (ids.isEmpty()) {
    DAQDevice *deflt = new DAQDevice("");
    devices.push_back(deflt);
  } else {
    int n=0;
    for (QStringList::iterator i=ids.begin(); i!=ids.end(); ++i) {
      QString id = *i;
      DAQDevice *dev = new DAQDevice(id);
      devices.push_back(dev);
      aliases[id] = n;
      n++;
    }
  }
  aliases[""] = 0;

}

QString DAQDevice::nidaqVersion() {
    uInt32 maj, min, upd;
    int e = DAQmxGetSysNIDAQMajorVersion(&maj);
    // daqTry(DAQmxGetSysNIDAQMajorVersion())
    if (e<0)
        return "(no driver)";
    else if (e>0)
        return "(error)";
    e = DAQmxGetSysNIDAQMinorVersion(&min);
    // daqTry(DAQmxGetSysNIDAQMinorVersion())
    if (e<0)
        return "(no driver)";
    else if (e>0)
        return "(error)";
    e = DAQmxGetSysNIDAQUpdateVersion(&upd);
    // daqTry(DAQmxGetSysNIDAQUpdateVersion())
    if (e<0)
        return "(no driver)";
    else if (e>0)
        return "(error)";
    return QString("%1.%2.%3").arg(maj).arg(min).arg(upd);
}
