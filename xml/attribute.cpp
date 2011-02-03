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
