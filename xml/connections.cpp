// connections.cpp

#include "connections.h"

#include <base/exception.h>
#include <xml/param.h>
#include <xml/attribute.h>
#include <base/dbg.h>

namespace Connections {
  QMap<QString,AIChannel *> aimap;
  QMap<QString,DigiChannel *> digimap;
  QMap<QString,CamCon *> cammap;

  AIChannel::AIChannel(QString id): id(id) {
    ground="NRSE";
    range = 5;
    scale = 1000;
    unit = "mV";
    alias = id;
  }
  
  CamCon::CamCon(QString id): id(id) {
    serno="-";
    partnerid="-";
    flipx = flipy = false;
    xpix = ypix = 512;
    focusexp_ms = 100;
  }
  
  DigiChannel::DigiChannel(QString id): id(id) {
    alias=id;
    out=false;
    in=true;
  }
  
  //  Connections() {
  //  }
  //  
  //  ~Connections() {
  //    for (QMap<QString,AIChannel *>::iterator i=aimap.begin();
  //         i!=aimap.end(); ++i)
  //      delete i.value();
  //    for (QMap<QString,CamCon *>::iterator i=cammap.begin();
  //         i!=cammap.end(); ++i)
  //      delete i.value();
  //  }
  
  void writeXML(QDomElement doc) {
    if (doc.tagName()!="connections") {
      QDomElement e = doc.firstChildElement("connections");
      if (e.isNull()) {
        e = doc.ownerDocument().createElement("connections");
        doc.appendChild(e);
      }
      doc = e;
    }
    
    for (QMap<QString,AIChannel *>::const_iterator i=aimap.begin();
         i!=aimap.end(); ++i) {
      AIChannel &aic = *(*i);
      QDomElement e = doc.ownerDocument().createElement("aichannel");
      doc.appendChild(e);
      e.setAttribute("id",aic.id);
      e.setAttribute("alias",aic.alias);
      e.setAttribute("ground",aic.ground);
      e.setAttribute("range",QString("%1V").arg(aic.range));
      e.setAttribute("unit",aic.unit);
      e.setAttribute("scale",QString("%1").arg(aic.scale));
    }
  
    for (QMap<QString,CamCon *>::const_iterator i=cammap.begin();
         i!=cammap.end(); ++i) {
      CamCon &cam = *(*i);
      QDomElement e = doc.ownerDocument().createElement("camera");
      doc.appendChild(e);
      e.setAttribute("id",cam.id);
      e.setAttribute("partnerid",cam.partnerid);
      e.setAttribute("serno",cam.serno);
      e.setAttribute("xpix",QString::number(cam.xpix));
      e.setAttribute("ypix",QString::number(cam.ypix));
      e.setAttribute("flipx",cam.flipx?"yes":"no");
      e.setAttribute("flipy",cam.flipy?"yes":"no");
      Param expo("time"); expo.setDouble(cam.focusexp_ms);
      e.setAttribute("focusexpose",expo.toString());
    }
  
    for (QMap<QString,DigiChannel *>::const_iterator i=digimap.begin();
         i!=digimap.end(); ++i) {
      DigiChannel &dig = *(*i);
      QDomElement e = doc.ownerDocument()
        .createElement(dig.out?"dochannel":"dichannel");
      doc.appendChild(e);
      e.setAttribute("id",dig.id);
      e.setAttribute("alias",dig.alias);
    }
  }
  
  void readXML(QDomElement doc) {
    if (!aimap.isEmpty() || !digimap.isEmpty() || !cammap.isEmpty())
      dbg("Connections: Warning: Replacing old connections with new");

    for (QMap<QString,AIChannel *>::iterator i=aimap.begin();
         i!=aimap.end(); ++i)
      delete i.value();
    aimap.clear();
    for (QMap<QString,DigiChannel *>::iterator i=digimap.begin();
         i!=digimap.end(); ++i)
      delete i.value();
    digimap.clear();
    for (QMap<QString,CamCon *>::iterator i=cammap.begin();
         i!=cammap.end(); ++i)
      delete i.value();
    cammap.clear();
    
    if (doc.tagName()!="connections")
      doc = doc.firstChildElement("connections");
    if (doc.isNull())
      throw Exception("Connections","No <connections> element","read");
  
    // Read AI CHANNELS
    for (QDomElement e=doc.firstChildElement("aichannel");
         !e.isNull(); e=e.nextSiblingElement("aichannel")) {
      QString id = xmlAttribute(e,"id");
      AIChannel *aic = new AIChannel(id);
      aimap[id] = aic;
      if (e.hasAttribute("ground"))
        aic->ground = e.attribute("ground");
      if (e.hasAttribute("alias"))
        aic->alias = e.attribute("alias");
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
      if (e.hasAttribute("unit"))
        aic->unit = e.attribute("unit");
    }
  
    // Read CAMERAS
    for (QDomElement e=doc.firstChildElement("camera");
         !e.isNull(); e=e.nextSiblingElement("camera")) {
      QString id = xmlAttribute(e,"id");
      CamCon *cam = new CamCon(id);
      cammap[id]=cam;
      cam->serno = e.attribute("serno");
      cam->partnerid = e.attribute("partnerid");
      cam->xpix = e.attribute("xpix").toInt();
      cam->ypix = e.attribute("ypix").toInt();
      QString flipx = e.attribute("flipx");
      cam->flipx = flipx=="true" || flipx=="on" || flipx=="yes";
      QString flipy = e.attribute("flipy");
      cam->flipy = flipy=="true" || flipy=="on" || flipy=="yes";
      Param expo("time"); expo.set(e.attribute("focusexpose"));
      cam->focusexp_ms = expo.toDouble();
    }
  
    // Read DI CHANNELS
    for (QDomElement e=doc.firstChildElement("dichannel");
         !e.isNull(); e=e.nextSiblingElement("dichannel")) {
      QString id = xmlAttribute(e,"id");
      DigiChannel *dig = new DigiChannel(id);
      digimap[id]=dig;
      if (e.hasAttribute("alias"))
        dig->alias = e.attribute("alias");
      dig->in = true;
      dig->out = false;
    }
  
    // Read DO CHANNELS
    for (QDomElement e=doc.firstChildElement("dochannel");
         !e.isNull(); e=e.nextSiblingElement("dochannel")) {
      QString id = xmlAttribute(e,"id");
      DigiChannel *dig = new DigiChannel(id);
      digimap[id]=dig;
      if (e.hasAttribute("alias"))
        dig->alias = e.attribute("alias");
      dig->in = false;
      dig->out = true;
    }
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
  
  QStringList allCams() {
    QStringList sl;
    for (QMap<QString,CamCon *>::const_iterator k=cammap.begin();
         k!=cammap.end(); k++)
      sl.append(k.key());
    return sl;
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
  
  QStringList digiOutputLines() {
    QStringList l;
    for (QMap<QString,DigiChannel *>::const_iterator i=digimap.begin();
         i!=digimap.end(); ++i) {
      DigiChannel const *c = i.value();
      if (c->out)
        l.push_back(c->id);
    }
    return l;
  }
    
  QStringList digiInputLines() {
    QStringList l;
    //dbg("connections:digiinputlines:");
    for (QMap<QString,DigiChannel *>::const_iterator i=digimap.begin();
         i!=digimap.end(); ++i) {
      DigiChannel const *c = i.value();
      //dbg("line id %s / %s",qPrintable(c->id),c->in? "in":"out");
      if (c->in)
        l.push_back(c->id);
    }
    return l;
  }
}
