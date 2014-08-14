// param.cpp

#include <xml/param.h>
#include <xml/enumerator.h>
#include <xml/attribute.h>
#include <xml/xmlfind.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <math.h>
#include <QRegExp>
#include <stdio.h>
#include <QSet>

Param::Param(QString type, QString enumname) {
  newType(type, enumname);
  cond=0;
  deflt=value;
  immune = false;
}

Param::Param(QDomElement defn) {
  newDefn(defn);
  cond=0;
  deflt=value;
}

void Param::newDefn(QDomElement defn) {
  QString type = xmlAttribute(defn, "type");
  QString enumname="";
  if (type=="enum")
    enumname=xmlAttribute(defn, "enum");
  newType(type, enumname);
  if (defn.hasAttribute("default")) {
    try {
      set(xmlAttribute(defn, "default"));
    } catch (Exception const &) {
      Dbg() << "Param caught exception: '" << xmlAttribute(defn,"default")
	    << "' is not an acceptable value for "<< type  << "." << enumname;
    }
  }
  if (defn.hasAttribute("min")) {
    min = new Param(type, enumname);
    min->set(xmlAttribute(defn, "min"));
  } else {
    min = 0;
  }
  if (defn.hasAttribute("max")) {
    max = new Param(type, enumname);
    max->set(xmlAttribute(defn, "max"));
  } else {
    max = 0;
  }
  immune = defn.hasAttribute("immune") && defn.attribute("immune").toInt();
}

Param::Param(QDomElement doc, QString path) {
  newDefn(xmlFindParam(doc,path));
  cond=0;
  deflt=value;
}

Param::~Param() {
  /*
    We do not delete the min, max, and cond, because we may have shallow
    copies doubling around. A QSharedPointer could fix this memory leak.
  */
}

void Param::newType(QString type, QString enumname) {
  min=max=0;
  valueType = type;
  if (!(valueType=="" ||
	valueType=="double" ||
	valueType=="int" ||
	valueType=="bool" ||
	valueType=="percentage" ||
	valueType=="time" ||
	valueType=="freq" ||
	valueType=="voltage" ||
	valueType=="current" ||
	valueType=="geometry" ||
	valueType=="string" ||
	valueType=="enum" ||
	valueType=="set"))
    throw Exception("Param",
		    QString("Unknown value type '") + type +"'",
		    "constructor");
  if (valueType=="enum")
    enumerator = Enumerator::find(enumname);
  else
    enumerator = 0;
}

void Param::set(QString s) {
  bool ok = true;
  QVariant v;
  if (valueType=="string") {
    v = QVariant(s);
  } else if (valueType=="double") {
    v = QVariant(s.toDouble(&ok));
  } else if (valueType=="int") {
    v = QVariant(s.toInt(&ok));
  } else if (valueType=="bool") {
    if (s=="on" || s=="yes" || s=="true" || s=="1")
      v = QVariant(true);
    else if (s=="off" || s=="no" || s=="false" || s=="0")
      v = QVariant(false);
    else
      ok = false;
  } else if (valueType=="percentage") {
    QRegExp re("^([0-9-.]+) *%$");
    if (re.indexIn(s)>-1) 
      v = re.cap(1).toDouble(&ok);
    else
      ok = false;
  } else if (valueType=="time") {
    QRegExp re("^([0-9-.]+)\\s*(|s|ms|min|h|us)$");
    if (re.indexIn(s)>-1) {
      double val = re.cap(1).toDouble(&ok);
      QString unit = re.cap(2);
      if (unit=="ms" || unit=="")
	;
      else if (unit=="us")
	val/=1000;
      else if (unit=="s")
	val*=1000;
      else if (unit=="min")
	val*=60*1000;
      else if (unit=="h")
	val*=60*60*1000;
      v = QVariant(val);
    } else {
      throw Exception("Param","Time must be expressed in us, ms, s, min, or h");
    }
  } else if (valueType=="freq") {
    QRegExp re("^([0-9-.]+)\\s*(|fps|Hz|kHz)$");
    if (re.indexIn(s)>-1) {
      double val = re.cap(1).toDouble(&ok);
      QString unit = re.cap(2);
      if (unit=="Hz" || unit=="fps" || unit=="")
	;
      else if (unit=="kHz")
	val*=1000;
      v = QVariant(val);
    } else {
      throw Exception("Param","Frequency must be expressed in Hz or kHz");
    }
  } else if (valueType=="voltage") {
    QRegExp re("^([0-9-.]+)\\s*(|uV|mV|V)$");
    if (re.indexIn(s)>-1) {
      double val = re.cap(1).toDouble(&ok);
      QString unit = re.cap(2);
      if (unit=="mV" || unit=="")
	;
      else if (unit=="V")
	val*=1000;
      else if (unit=="uV")
	val/=1000;
      v = QVariant(val);
    } else {
      throw Exception("Param",
		      "Voltage must be expressed in uV, mV, or V for "
		      + dbgPath);
    }
  } else if (valueType=="current") {
    QRegExp re("^([0-9-.]+)\\s*(|pA|nA|uA|mA)$");
    if (re.indexIn(s)>-1) {
      double val = re.cap(1).toDouble(&ok);
      QString unit = re.cap(2);
      if (unit=="nA" || unit=="")
	;
      else if (unit=="uA")
	val*=1000;
      else if (unit=="mA")
	val*=1000*1000;
      else if (unit=="pA")
	val/=1000;
      v = QVariant(val);
    } else {
      throw Exception("Param","Current must be expressed in pA, nA, uA, or mA");
    }
  } else if (valueType=="geometry") {
    QRegExp re("^(\\d+)x(\\d+)\\+(\\d+)\\+(\\d+)$");
    if (re.indexIn(s)>-1) {
      int w = re.cap(1).toInt();
      int h = re.cap(2).toInt();
      int x0 = re.cap(3).toInt();
      int y0 = re.cap(4).toInt();
      v = QVariant(QRect(x0,y0,w,h));
    } else {
      throw Exception("Param","Geometry must be expressed as WxH+X0+Y0 (not '"+s+"')");
    }
  } else if (valueType=="enum") {
    try {
      v = QVariant(enumerator->lookup(s));
    } catch(Exception const &) {
      v = QVariant(enumerator->getLargestValue());
      throw;
    }
  } else if (valueType=="set") {
    if (s.left(1)=="+") {
      QSet<QString> vals = QSet<QString>::fromList(value.toString().split(" "));
      vals.insert(s.mid(1));
      v = QVariant(QStringList(vals.toList()).join(" "));
    } else if (s.left(1)=="-") {
      QSet<QString> vals = QSet<QString>::fromList(value.toString().split(" "));
      vals.remove(s.mid(1));
      v = QVariant(QStringList(vals.toList()).join(" "));
    } else {
      QStringList l = s.split(QRegExp("\\s+"),QString::SkipEmptyParts);
      v=QVariant(l.join(" "));
    }
  } else {
    throw Exception("Param", QString("Unknown value type"),"set");
  }
  if (!ok)
    throw Exception("Param",
		    QString("Bad value '%1' for type '%2'")
		    .arg(s)
		    .arg(valueType));
  rangeCheck(v);
}

void Param::setStrings(QSet<QString> const &ss) {
  if (valueType=="set") {
    QVariant v = QVariant(QStringList(ss.toList()).join(" "));
    rangeCheck(v);
  } else {
    throw Exception("Param", "Only sets can be defined by sets of strings");
  }
}

void Param::rangeCheck(QVariant const &v) {
  bool ok=true;
  if (min || max) {
    bool canConvert;
    double f = v.toDouble(&canConvert);
    if (canConvert) {
      if (min) {
	double x = min->toDouble();
	if (f<x)
	  ok=false;
      }
      if (max) {
	double x = max->toDouble();
	if (f>x)
	  ok=false;
      }
    }
  }
  if (ok) {
    value=v;
  } else {
    Dbg() << "Param " << dbgPath 
	  << " value " << v.toDouble() << " out of bounds";
    throw Exception("Param","Value out of bounds");
  }
}

void Param::setRect(QRect const &r) {
  if (valueType=="geometry")
    value = QVariant(r);
  else
    throw Exception("Param","Only geometry can be set as QRect");
}

QRect Param::toRect() const {
  if (valueType=="geometry")
    return value.toRect();
  else
    throw Exception("Param","Only geometry can be read as QRect");
}

void Param::setInt(int i) {
  QVariant v;
  if (valueType=="int")
    v=QVariant(i);
  else if (valueType=="bool")
    v=QVariant(i!=0);
  else if (valueType=="double")
    v=QVariant(double(i));
  else if (valueType=="enum") {
    enumerator->reverseLookup(i); // ensure this number exists for us
    v=QVariant(i);
  } else {
    throw Exception("Param", QString("Cannot make '") + valueType +"' from integer");
  }
  rangeCheck(v);
}

void Param::setBool(bool b) {
  QVariant v;
  if (valueType=="bool")
    v=QVariant(b);
  else
    throw Exception("Param", QString("Cannot make '") + valueType +"' from boolean");
  rangeCheck(v);
}
  
void Param::setDouble(double f) {
  QVariant v;
  if (valueType=="int")
    v=QVariant(int(f));
  else if (valueType=="bool")
    v=QVariant(f!=0);
  else if (valueType=="double")
    v=QVariant(f);
  else if (valueType=="time")
    v=QVariant(f); // assume ms.
  else if (valueType=="freq")
    v=QVariant(f); // assume Hz.
  else if (valueType=="voltage")
    v=QVariant(f); // assume mV.
  else if (valueType=="current")
    v=QVariant(f); // assume nA.
  else 
    throw Exception("Param", QString("Cannot make '") + valueType +"' from integer");
  rangeCheck(v);
}

int Param::toInt() const {
  bool ok;
  int r = value.toInt(&ok);
  if (ok)
    return r;
  else
    throw Exception("Param",QString("Cannot convert ") + valueType +" '" + toString() + "' to integer");
}

double Param::toDouble() const {
  bool ok;
  double r = value.toDouble(&ok);
  if (ok)
    return r;
  else
    throw Exception("Param",QString("Cannot convert '") + valueType +"' to double");
}

bool Param::toBool() const {
  return value.toBool();
}  

QSet<QString> Param::toStrings() const {
  if (valueType!="set")
    throw Exception("Param","Only sets can be represented as QSet<QString>");
  QStringList ss = value.toString().split(" ");
  return QSet<QString>::fromList(ss);
}

QString Param::toString() const {
  if (valueType=="enum") {
    return enumerator->reverseLookup(value.toInt());
  } else if (valueType=="set") {
    return value.toString();
  } else if (valueType=="time") {
    double v = value.toDouble();
    if (fabs(v)>=180*1000) 
      return QString("%1 min").arg(v/60/1000);
    else if (fabs(v)>=1000)
      return QString("%1 s").arg(v/1000);
    else if (fabs(v)>=1)
      return QString("%1 ms").arg(v);
    else
      return QString("%1 us").arg(v*1000);      
  } else if (valueType=="freq") {
    double v = value.toDouble();
    if (fabs(v)>=1000) 
      return QString("%1 kHz").arg(v/1000);
    else
      return QString("%1 Hz").arg(v);
  } else if (valueType=="voltage") {
    double v = value.toDouble();
    if (fabs(v)>=1000) 
      return QString("%1 V").arg(v/1000);
    else if (fabs(v)>=1)
      return QString("%1 mV").arg(v);
    else
      return QString("%1 uV").arg(v*1000);      
  } else if (valueType=="current") {
    double v = value.toDouble();
    if (fabs(v)>=200000) 
      return QString("%1 mA").arg(v/1000000);
    if (fabs(v)>=200) 
      return QString("%1 uA").arg(v/1000);
    else if (fabs(v)>=0.2)
      return QString("%1 nA").arg(v);
    else
      return QString("%1 pA").arg(v*1000);      
  } else if (valueType=="geometry") {
    QRect r = value.toRect();
    return QString("%1x%2+%3+%4").
      arg(r.width()).arg(r.height()).arg(r.left()).arg(r.top());
  } else if (valueType=="percentage") {
    return value.toString() + "%";
  } else {
    return value.toString();
  }
}

Param::Param(Param const &src) {
  value=src.value;
  valueType=src.valueType;
  enumerator=src.enumerator;
  min=src.min; // note shallow copy
  max=src.max; // note shallow copy
  cond = src.cond; // note shallow and naive copy
  enable_if = src.enable_if;
  deflt = src.deflt;
  immune = src.immune;
}

Param &Param::operator=(Param const &src) {
  if (src.valueType!=valueType)
    throw Exception("Param","Attempt to assign param with incompatible type");
  if (valueType=="enum" && src.enumerator!=enumerator)
    throw Exception("Param","Attempt to assign param with incompatible enum");
  value = src.value;
  min=src.min; // note shallow copy
  max=src.max; // note shallow copy
  cond = src.cond; // note shallow and naive copy
  enable_if = src.enable_if; 
  deflt = src.deflt;
  immune = src.immune;
  return *this;
}


void Param::report() const {
  printf("Param report: %s. type=%s. ",
	 qPrintable(dbgPath), qPrintable(valueType));
  if (enumerator)
    printf("enum=%s. ", qPrintable(enumerator->getName()));
  printf("value=%s. default=%s. ",
	 qPrintable(toString()), qPrintable(deflt.toString()));
  if (!enable_if.isEmpty())
    printf("enable-if: %s. ",qPrintable(enable_if.join(", ")));
  printf("\n");
  if (min) {
    printf("  Min: ");
    min->report();
  }
  if (max) {
    printf("  Max: ");
    max->report();
  }
  if (cond) {
    printf("  Cond: ");
    cond->report();
  }
}

void Param::read(QDomElement v) {
  // We assume v is a <pval>.
  set(xmlAttribute(v,"value","Param","Cannot read value"));
  if (false && v.hasAttribute("default"))
    deflt = QVariant(v.attribute("default"));
}

void Param::write(QDomElement v) {
  v.setAttribute("value",toString());
}

bool Param::read(QDomElement doc, QString path) {
  bool exist;
  QDomElement v = xmlFindPval(doc,path,false,&exist);
  if (exist)
    read(v);
  return exist;
}

void Param::write(QDomElement doc, QString path) {
  QDomElement v = xmlFindPval(doc,path,true);
  write(v);
}
  
bool Param::operator==(Param const &oth) const {
  if (valueType!=oth.valueType)
    return false;
  if (valueType=="enum" || valueType=="set")
    if (enumerator!=oth.enumerator)
      return false;
  return value==oth.value;
}

bool Param::isEnabled() const {
  if (cond) {
    QString v = cond->toString();
    bool r = enable_if.contains(v);
    //fprintf(stderr,"Param(%s)::isEnabled. cond(%s)=%s list=(%s). -> %c\n",
    //        qPrintable(dbgPath),qPrintable(cond->dbgPath),
    //        qPrintable(v), qPrintable(enable_if.join(", ")),
    //        r ? 'y' : 'n');
    return r;
  } else {
    if (enable_if.contains("*never"))
      return false;
    else
      return true;
  }
}

void Param::restore() {
  value = deflt;
}

void Param::setEnabler(Param *cond_, QString enable_if_) {
  cond = cond_;
  enable_if = enable_if_.split(" ");
}

void Param::setEnabler(Param *cond_) {
  cond = cond_;
}

bool Param::isImmune() const {
  return immune;
}
