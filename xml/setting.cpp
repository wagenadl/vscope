// setting.cpp

#include <xml/setting.h>
#include <xml/enumerator.h>
#include <base/exception.h>
#include <math.h>
#include <QRegExp>

Setting::Setting(QString type, QString enumname) {
  newType(type,enumname);
}

void Setting::newType(QString type, QString enumname) {
  valueType = type;
  if (!(valueType=="" ||
	valueType=="double" ||
	valueType=="int" ||
	valueType=="bool" ||
	valueType=="time" ||
	valueType=="freq" ||
	valueType=="geometry" ||
	valueType=="enum"))
    throw Exception("Setting",QString("Unknown value type '") + type +"'","constructor");
  if (valueType=="enum")
    enumerator = Enumerator::find(enumname);
  else
    enumerator = 0;
}

void Setting::set(QString s) {
  bool ok = false;
  if (valueType=="double") {
    value = QVariant(s.toDouble(&ok));
  } else if (valueType=="int") {
    value = QVariant(s.toInt(&ok));
  } else if (valueType=="bool") {
    if (s=="on" || s=="yes" || s=="true" || s=="1")
      value = QVariant(true);
    else if (s=="off" || s=="no" || s=="false" || s=="0")
      value = QVariant(false);
    else
      throw Exception("Setting","Bad value for bool");
  } else if (valueType=="time") {
    QRegExp re("^([0-9-.]+)\\s*(|s|ms|min|h|us)$");
    if (re.indexIn(s)>-1) {
      double val = re.cap(1).toDouble(&ok);
      QString unit = re.cap(2);
      if (unit=="ms" || unit=="")
	;
      else if (unit=="us")
	val*=1000;
      else if (unit=="s")
	val/=1000;
      else if (unit=="min")
	val*=60*1000;
      else if (unit=="h")
	val*=60*60*1000;
      value = QVariant(val);
    } else {
      throw Exception("Setting","Time must be expressed in us, ms, s, min, or h");
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
      value = QVariant(val);
    } else {
      throw Exception("Setting","Frequency must be expressed in Hz or kHz");
    }
  } else if (valueType=="geometry") {
    QRegExp re("^(\\d+)x(\\d+)+(\\d+)+(\\d+)$");
    if (re.indexIn(s)>-1) {
      int w = re.cap(1).toInt();
      int h = re.cap(2).toInt();
      int x0 = re.cap(3).toInt();
      int y0 = re.cap(4).toInt();
      value = QVariant(QRect(x0,y0,w,h));
    } else {
      throw Exception("Setting","Geometry must be expressed as WxH+X0+Y0");
    }
  } else if (valueType=="enum") {
    value = QVariant(enumerator->lookup(s));
  } else {
    throw Exception("Setting", QString("Unknown value type"),"set");
  }
}

void Setting::setRect(QRect const &r) {
  if (valueType=="geometry")
    value = QVariant(r);
  else
    throw Exception("Setting","Only geometry can be set as QRect");
}

QRect Setting::toRect() const {
  if (valueType=="geometry")
    return value.toRect();
  else
    throw Exception("Setting","Only geometry can be read as QRect");
}

void Setting::setInt(int i) {
  if (valueType=="int")
    value=QVariant(i);
  else if (valueType=="bool")
    value=QVariant(i!=0);
  else if (valueType=="double")
    value=QVariant(double(i));
  else if (valueType=="enum") {
    enumerator->reverseLookup(i); // ensure this number exists for us
    value=QVariant(i);
  } else {
    throw Exception("Setting", QString("Cannot make '") + valueType +"' from integer");
  }
}

void Setting::setDouble(double f) {
  if (valueType=="int")
    value=QVariant(int(f));
  else if (valueType=="bool")
    value=QVariant(f!=0);
  else if (valueType=="double")
    value=QVariant(f);
  else if (valueType=="time")
    value=QVariant(f); // assume ms.
  else if (valueType=="freq")
    value=QVariant(f); // assume Hz.
  else 
    throw Exception("Setting", QString("Cannot make '") + valueType +"' from integer");
}

int Setting::toInt() const {
  bool ok;
  int r = value.toInt(&ok);
  if (ok)
    return r;
  else
    throw Exception("Setting",QString("Cannot convert '") + valueType +"' to integer");
}

double Setting::toDouble() const {
  bool ok;
  double r = value.toDouble(&ok);
  if (ok)
    return r;
  else
    throw Exception("Setting",QString("Cannot convert '") + valueType +"' to double");
}

bool Setting::toBool() const {
  return value.toBool();
}  

QString Setting::toString() const {
  if (valueType=="enum") {
    return enumerator->reverseLookup(value.toInt());
  } else if (valueType=="time") {
    double v = value.toDouble();
    if (fabs(v)>=180*1000) 
      return QString("%1 min").arg(v/60/1000);
    else if (fabs(v)>1000)
      return QString("%1 s").arg(v/1000);
    else
      return QString("%1 ms").arg(v);
  } else if (valueType=="freq") {
    double v = value.toDouble();
    if (fabs(v)>=1000) 
      return QString("%1 kHz").arg(v/1000);
    else
      return QString("%1 Hz").arg(v);
  } else if (valueType=="geometry") {
    QRect r = value.toRect();
    return QString("%1x%2+%3+%4").arg(r.width()).arg(r.height()).arg(r.left()).arg(r.top());
  } else {
    return value.toString();
  }
}

Setting &Setting::operator=(Setting const &src) {
  if (src.valueType!=valueType)
    throw Exception("Setting","Attempt to assign setting with incompatible type");
  if (valueType=="enum" && src.enumerator!=enumerator)
    throw Exception("Setting","Attempt to assign setting with incompatible enum");
  value = src.value;
  return *this;
}
