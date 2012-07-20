// attribute.cpp

#include <xml/attribute.h>
#include <base/exception.h>

QString xmlAttribute(QDomElement e, QString tag,
		     QString issuer, QString errmsg) {
  if (e.hasAttribute(tag))
    return e.attribute(tag);
  else
    throw Exception(issuer,errmsg);
}

QString xmlAttribute(QDomElement e, QString tag, QString dflt) {
  if (e.hasAttribute(tag))
    return e.attribute(tag);
  else
    return dflt;
}

bool attributeTrue(QDomElement e, QString tag) {
  QString v = xmlAttribute(e, tag).toLower();
  return v=="yes" || v=="on" || v=="true" || v=="1";
}

bool attributeFalse(QDomElement e, QString tag) {
  QString v = xmlAttribute(e, tag).toLower();
  return v=="no" || v=="off" || v=="false" || v=="0";
}

