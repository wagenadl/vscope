// updatecamenum.cpp

#include "updatecamenum.h"
#include <toplevel/globals.h>
#include <xml/enumerator.h>
#include <QSet>

void updateCamEnum(Enumerator *enu, QString pfx, QList<QString> const &nw) {
  QStringList oldids_l = enu->getNonnegativeTags();
  QSet<QString> oldids;
  foreach (QString id, oldids_l)
    if (id.startsWith(pfx))
      oldids.insert(id.mid(pfx.length()));
  
  QSet<QString> newids;
  foreach (QString id, nw) 
    newids.insert(id);

  foreach (QString id, oldids)
    if (!newids.contains(id))
      enu->remove(pfx+id);
  
  foreach (QString id, newids)
    if (!oldids.contains(id))
      enu->add(pfx+id);
}


  
