// xml/connections.cpp - This file is part of VScope.
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

// connections.cpp

#include "connections.h"

#include <base/exception.h>
#include <xml/param.h>
#include <xml/attribute.h>
#include <xml/enumerator.h>
#include <base/dbg.h>

namespace Connections {
  QMap<QString,AIChannel *> aimap;
  QStringList aiorder;
  QMap<QString,AOChannel *> aomap;
  QMap<QString,DigiChannel *> digimap;
  QMap<QString,CamCon *> cammap;
  QStringList camorder;
  QString devid, devserno, devtype;

  AIChannel::AIChannel(QString id): id(id) {
    line = -1;
    ground="NRSE";
    range = 5;
    scale = 1000;
    offset = 0;
    unit = "mV";
  }

  AOChannel::AOChannel(QString id): id(id) {
    line = -1;
    scale = 1000;
    unit = "mV";
  }

  CamCon::CamCon(QString id): id(id) {
    serno="-";
    partnerid="";
    xpix = ypix = 512;
    focusexp_ms = 100;
  }
  
  DigiChannel::DigiChannel(QString id): id(id) {
    line = -1;
    out=false;
    in=true;
  }
  
  void writeAOChannels(QDomElement doc) {
    foreach (AOChannel const *aoc, aomap.values()) {
      QDomElement e = doc.ownerDocument().createElement("aochannel");
      doc.appendChild(e);
      e.setAttribute("id",aoc->id);
      if (aoc->line>=0)
	e.setAttribute("line",QString("%1").arg(aoc->line));
      e.setAttribute("unit",aoc->unit);
      e.setAttribute("scale",QString("%1").arg(aoc->scale));
    }
  }

  void writeAIChannels(QDomElement doc) {
    for (QMap<QString,AIChannel *>::const_iterator i=aimap.begin();
         i!=aimap.end(); ++i) {
      AIChannel &aic = *(*i);
      QDomElement e = doc.ownerDocument().createElement("aichannel");
      doc.appendChild(e);
      e.setAttribute("id",aic.id);
      if (aic.line>=0)
	e.setAttribute("line",QString("%1").arg(aic.line));
      e.setAttribute("ground",aic.ground);
      e.setAttribute("range",QString("%1V").arg(aic.range));
      e.setAttribute("unit",aic.unit);
      e.setAttribute("scale",QString("%1").arg(aic.scale));
      e.setAttribute("offset",QString("%1").arg(aic.offset));
    }
  }

  void writeCameras(QDomElement doc) {
    foreach (QString id, camorder) {
      CamCon &cam = *cammap[id];
      QDomElement e = doc.ownerDocument().createElement("camera");
      doc.appendChild(e);
      e.setAttribute("id",cam.id);
      e.setAttribute("partnerid",cam.partnerid);
      e.setAttribute("shutter",cam.shtrid);
      e.setAttribute("trigger",cam.trigid);
      e.setAttribute("lamp",cam.lampid);
      e.setAttribute("serno",cam.serno);
      e.setAttribute("xpix",QString::number(cam.xpix));
      e.setAttribute("ypix",QString::number(cam.ypix));
      Param expo("time"); expo.setDouble(cam.focusexp_ms);
      e.setAttribute("focusexpose",expo.toString());
      e.setAttribute("role", cam.isdonor ? "donor"
		     : cam.isacceptor ? "acceptor"
		     : "");
      cam.placement.write(e);
    }
  }

  void writeDIOChannels(QDomElement doc) {
    for (QMap<QString,DigiChannel *>::const_iterator i=digimap.begin();
         i!=digimap.end(); ++i) {
      DigiChannel &dig = *(*i);
      QDomElement e = doc.ownerDocument()
        .createElement(dig.out?"dochannel":"dichannel");
      doc.appendChild(e);
      e.setAttribute("id",dig.id);
      if (dig.line>=0)
	e.setAttribute("line",QString("%1").arg(dig.line));
    }
  }

  void writeXML(QDomElement doc) {
    if (doc.tagName()!="connections") {
      QDomElement e = doc.firstChildElement("connections");
      if (e.isNull()) {
        e = doc.ownerDocument().createElement("connections");
        doc.appendChild(e);
      }
      doc = e;
    }

    writeAIChannels(doc);
    writeAOChannels(doc);
    writeDIOChannels(doc);
    writeCameras(doc);
  }

  void readDAQDevice(QDomElement doc) {
    devid = "";
    devserno = "";
    devtype = "";
    for (QDomElement e=doc.firstChildElement("device");
         !e.isNull(); e=e.nextSiblingElement("device")) {
      if (e.hasAttribute("id"))
        devid = e.attribute("id");
      if (e.hasAttribute("serno"))
        devserno = e.attribute("serno");
      if (e.hasAttribute("type"))
        devtype = e.attribute("type");
    }
  }
  
  void readAIChannels(QDomElement doc) {
    Enumerator *ai = Enumerator::find("AICHAN");
    for (QDomElement e=doc.firstChildElement("aichannel");
         !e.isNull(); e=e.nextSiblingElement("aichannel")) {
      QString id = xmlAttribute(e,"id");
      if (e.hasAttribute("line")) {
        aiorder << id;
	AIChannel *aic = new AIChannel(id);
	aimap[id] = aic;
	if (e.hasAttribute("ground"))
	  aic->ground = e.attribute("ground");
	aic->line = e.attribute("line").toInt();
	ai->add(id, aic->line);
	if (e.hasAttribute("range")) {
	  Param p("voltage");
	  p.set(e.attribute("range"));
	  aic->range = p.toDouble()/1000; // convert mV to V.
	}
	if (e.hasAttribute("scale")) {
	  Param p("double");
	  p.set(e.attribute("scale"));
	  aic->scale = p.toDouble();
	}
	if (e.hasAttribute("offset")) {
	  Param p("double");
	  p.set(e.attribute("offset"));
	  aic->offset = p.toDouble();
	}
	if (e.hasAttribute("unit"))
	  aic->unit = e.attribute("unit");
      }
    }
  }

    
  void readAOChannels(QDomElement doc) {
    Enumerator *ao = Enumerator::find("AOCHAN");
    Enumerator *stimchs = Enumerator::find("STIMCHS");
    for (QDomElement e=doc.firstChildElement("aochannel");
         !e.isNull(); e=e.nextSiblingElement("aochannel")) {
      QString id = xmlAttribute(e,"id");
      if (e.hasAttribute("line")) {
	AOChannel *aoc = new AOChannel(id);
	aomap[id] = aoc;
	aoc->line = e.attribute("line").toInt();
	ao->add(id, aoc->line);
	aoc->stim = attributeTrue(e, "isstim");
	if (aoc->stim)
	  stimchs->add(id);
	if (e.hasAttribute("scale")) {
	  Param p("double");
	  p.set(e.attribute("scale"));
	  aoc->scale = p.toDouble();
	}
	if (e.hasAttribute("unit"))
	  aoc->unit = e.attribute("unit");
      }
    }
  }

  void readCameras(QDomElement doc) {
    int order = 0;
    Enumerator *cam_enum = Enumerator::find("CAMERAS");
    for (QDomElement e=doc.firstChildElement("camera");
         !e.isNull(); e=e.nextSiblingElement("camera")) {
      QString id = xmlAttribute(e,"id");
      CamCon *cam = new CamCon(id);
      cammap[id]=cam;
      camorder.append(id);
      cam->order = order++;
      cam->exists = false;
      cam->serno = e.attribute("serno");
      cam->partnerid = e.attribute("partnerid");
      if (e.hasAttribute("trigger"))
        cam->trigid = e.attribute("trigger");
      else
        cam->trigid = id;
      cam->shtrid = e.attribute("shutter");
      cam->lampid = e.attribute("lamp");
      cam->xpix = e.attribute("xpix").toInt();
      cam->ypix = e.attribute("ypix").toInt();
      Param expo("time"); expo.set(e.attribute("focusexpose"));
      cam->focusexp_ms = expo.toDouble();
      QString role = e.attribute("role");
      cam->isdonor = role=="donor";
      cam->isacceptor = role=="acceptor";
      cam->placement.read(e);
      cam_enum->add(id);
    }
    
    // back connect camera pairs
    foreach (QString id, camorder) {
      CamCon *me = cammap[id];
      QString pid = me->partnerid;
      if (pid != "") {
	CamCon *partner = cammap[pid];
	if (partner->partnerid == "")
	  partner->partnerid = id;
	else if (partner->partnerid != id)
	  throw Exception("Connections", "Cameras " + id + " and " + pid
			  + " have inconsistent partnering");
      }
    }
  }

  void readDIChannels(QDomElement doc) {
    Enumerator *dio = Enumerator::find("DIGILINES");
    for (QDomElement e=doc.firstChildElement("dichannel");
         !e.isNull(); e=e.nextSiblingElement("dichannel")) {
      QString id = xmlAttribute(e,"id");
      if (e.hasAttribute("line")) {
	DigiChannel *dig = new DigiChannel(id);
	digimap[id]=dig;
	dig->line = e.attribute("line").toInt();
	dio->add(id,dig->line);
	dig->in = true;
	dig->out = false;
      }
    }
  }

  void readDOChannels(QDomElement doc) {
    Enumerator *dio = Enumerator::find("DIGILINES");
    Enumerator *stimchs = Enumerator::find("STIMCHS");
    for (QDomElement e=doc.firstChildElement("dochannel");
         !e.isNull(); e=e.nextSiblingElement("dochannel")) {
      QString id = xmlAttribute(e,"id");
      if (e.hasAttribute("line")) {
	DigiChannel *dig = new DigiChannel(id);
	digimap[id]=dig;
	dig->line = e.attribute("line").toInt();
	dio->add(id,dig->line);
	dig->in = false;
	dig->out = true;
	dig->stim = attributeTrue(e, "isstim");
	if (dig->stim)
	  stimchs->add(id);
      }
    }
  }

  void readXML(QDomElement doc) {
    if (!aimap.isEmpty() || !digimap.isEmpty() || !cammap.isEmpty())
      dbg("Connections: Warning: Replacing old connections with new");

    foreach (AIChannel *aic, aimap.values())
      delete aic;
    aimap.clear();
    foreach (AOChannel *aoc, aomap.values())
      delete aoc;
    aomap.clear();
    foreach (DigiChannel *dc, digimap.values())
      delete dc;
    digimap.clear();
    foreach (CamCon *cc, cammap.values())
      delete cc;
    cammap.clear();
    
    if (doc.tagName()!="connections")
      doc = doc.firstChildElement("connections");
    if (doc.isNull())
      throw Exception("Connections","No <connections> element","read");

    readDAQDevice(doc);
    readAIChannels(doc);
    readAOChannels(doc);
    readDIChannels(doc);
    readDOChannels(doc);
    readCameras(doc);
  }

  
  AIChannel const *findpAI(QString id) {
    if (aimap.contains(id))
      return aimap[id];
    else
      return 0;
  }
  
  AIChannel const &findAI(QString id) {
    AIChannel const *aic = findpAI(id);
    if (aic)
      return *aic;
    throw Exception("Connections",
  		  "There is no channel named '" + id + "'","findAI");
  }
  
  AOChannel const *findpAO(QString id) {
    if (aomap.contains(id))
      return aomap[id];
    else
      return 0;
  }
  
  AOChannel const &findAO(QString id) {
    AOChannel const *aoc = findpAO(id);
    if (aoc)
      return *aoc;
    throw Exception("Connections",
  		  "There is no channel named '" + id + "'","findAO");
  }
  
  CamCon const *findpCam(QString id) {
    if (cammap.contains(id))
      return cammap[id];
    else
      return 0;
  }
  
  CamCon const &findCam(QString id) {
    CamCon const *cam = findpCam(id);
    if (cam)
      return *cam;
    throw Exception("Connections",
  		  "There is no camera named '" + id + "'","findCam");
  }

  CamPair camPair(QString id) {
    CamPair p;
    CamCon const &me = findCam(id);
    if (me.partnerid.isEmpty()) {
      p.donor = id;
      p.acceptor = "";
    } else if (me.isdonor) {
      p.donor = id;
      p.acceptor = me.partnerid;
    } else {
      p.donor = me.partnerid;
      p.acceptor = id;
    }
    return p;
  }

  void markCameraExists(QString id, bool exists) {
    if (cammap.contains(id))
      cammap[id]->exists = exists;
  }
  
  QStringList allCams() {
    return camorder;
  }

  QStringList donorCams() {
    QStringList sl;
    foreach (QString id, camorder) 
      if (cammap[id]->isdonor)
	sl.append(id);
    return sl;
  }

  QString leaderCamera() {
    foreach (QString id, camorder) 
      if (cammap[id]->isdonor && cammap[id]->exists)
	return id;
    foreach (QString id, camorder) 
      if (cammap[id]->exists)
	return id;
    foreach (QString id, camorder) 
      if (cammap[id]->isdonor)
	return id;
    foreach (QString id, camorder) 
      return id;
    return "";
  }

  CamPair leaderCamPair() {
    CamPair cp;
    foreach (QString id, camorder) {
      if (cammap[id]->isdonor && cammap[id]->exists) {
	cp.donor = id;
	cp.acceptor = cammap[id]->partnerid;
	return cp;
      }
    }
    foreach (QString id, camorder) {
      if (cammap[id]->exists) {
	cp.acceptor = id;
	cp.donor = cammap[id]->partnerid;
	return cp;
      }
    }
    foreach (QString id, camorder) {
      if (cammap[id]->isdonor) {
	cp.donor = id;
	cp.acceptor = cammap[id]->partnerid;
	return cp;
      }
    }
    foreach (QString id, camorder) {
      cp.acceptor = id;
      cp.donor = cammap[id]->partnerid;
      return cp;
    }
    return cp;
  }
   
  
  DigiChannel const *findpDig(QString id) {
    if (digimap.contains(id))
      return digimap[id];
    else
      return 0;
  }
  
  DigiChannel const &findDig(QString id) {
    DigiChannel const *dig = findpDig(id);
    if (dig)
      return *dig;
    throw Exception("Connections",
  		  "There is no di/do channel named '" + id + "'","findDig");
  }
  
  QStringList digiInputLines() {
    QStringList l;
    foreach (DigiChannel const *ch, digimap.values())
      if (ch->in)
	l.append(ch->id);
    return l;
  }

  QStringList digiOutputLines() {
    QStringList l;
    foreach (DigiChannel const *ch, digimap.values())
      if (ch->out)
	l.append(ch->id);
    return l;
  }

  QStringList digiStimLines() {
    QStringList l;
    foreach (DigiChannel const *ch, digimap)
      if (ch->out && ch->stim)
	l.append(ch->id);
    return l;
  }
  
  QStringList analogInputs() {
    return aiorder;
  }

  QStringList analogOutputs() {
    return aomap.keys();
  }

  QStringList analogStims() {
    QStringList l;
    foreach (AOChannel *ao, aomap)
      if (ao->stim)
	l.append(ao->id);
    return l;
  }

  QString deviceID() {
    return devid;
  }
  QString deviceSerNo() {
    return devserno;
  }
  QString deviceType() {
    return devtype;
  }
  
}
