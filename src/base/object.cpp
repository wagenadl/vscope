// object.cpp

#include "object.h"

QString objName(QObject const *obj) {
  QString n = obj->objectName();
  if (n.isEmpty())
    return obj->metaObject()->className() + QString("@0x")
      + QString::number((long int)(void*)obj, 16);
  else
    return n;
}
