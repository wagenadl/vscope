// toplevel/updatecamenum.cpp - This file is part of VScope.
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


  
