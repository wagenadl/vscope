// base/keyagg.h - This file is part of VScope.
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

// keyagg.h

#ifndef KEYAGG_H

#define KEYAGG_H

#include <base/keyaccess.h>

class KeyAgg: public KeyAccess {
  /*:C KeyAgg
   *:D Helper to avoid redundant calls to updateData() slots.
       If your class depends on multiple data sources, you can register
       those with a KeyAgg, and connect to the KeyAgg's newData() signal
       rather than to each of the sources' signals. That way, when data
       for more than one source is being updated, you only get signaled
       once the final key is checked in.
   */
  Q_OBJECT;
public:
  KeyAgg(QObject *parent);
  virtual ~KeyAgg();
  void add(KeyAccess *);
  void remove(KeyAccess *);
protected:
  virtual void emitUnlessCheckedOut();
  /*:F emitUnlessCheckedOut
   *:D Emits newData unless either we or any of our children has outstanding
       keys.
  */
private slots:
  void updateData();
  void sourceDying(QObject *);
private:
  QSet<KeyAccess *> sources;
};

#endif
