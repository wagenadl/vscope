// base/idkeyaccess.h - This file is part of VScope.
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

// idkeyaccess.h

#ifndef IDKEYACCESS_H

#define IDKEYACCESS_H

#include <base/keyaccess.h>
#include <QMultiMap>
#include <QMap>
#include <QSet>

class IDKeyAccess: public KeyAccess {
  Q_OBJECT;
public:
  //class ID {
  //public:
  //  ID(int id=0): id(id) { }
  //  operator int() const { return id; };
  //private:
  //  int id;
  //};
public:
  IDKeyAccess(QObject *parent=0);
  virtual ~IDKeyAccess();
  virtual WriteKey *checkout(int);
  virtual WriteKey *checkoutAll();
  virtual void checkin(WriteKey *);
  /*:F checkin
   *:D If the key was checked out with an ID, we emit newDatum(ID).
       If the key was checked out with checkoutAll, we emit newAll().
       In both cases, we also emit newData().
   */
  void verifyIDKey(WriteKey *key, int id, QString msg="") const;
  void verifyAllKey(WriteKey *key, QString msg="") const;
  int getID(WriteKey *key) const;
  /*:F getID
   *:D Returns the ID associated with the given key.
   *:N Throws an acception if the key is unknown or was not created by
       checkout(ID).
   */
signals:
  void newDatum(int id);
  void newAll();
private:
  QMap<WriteKey *, int> key2id;
  QMultiMap<int, WriteKey *> id2key;
  QSet<WriteKey *> key4all;
};

class IDKeyGuard {
public:
  IDKeyGuard(IDKeyAccess &src, int id);
  ~IDKeyGuard();
  KeyAccess::WriteKey *key() const { return key_; }
private:
  IDKeyAccess &src;
  IDKeyAccess::WriteKey *key_;
};

class AllKeyGuard {
public:
  AllKeyGuard(IDKeyAccess &src);
  ~AllKeyGuard();
  KeyAccess::WriteKey *key() const { return key_; }
private:
  IDKeyAccess &src;
  IDKeyAccess::WriteKey *key_;
};


  

#endif
