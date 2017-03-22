// base/idkeyaccess.cpp - This file is part of VScope.
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

// idkeyaccess.cpp

#include "idkeyaccess.h"
#include <base/exception.h>

IDKeyAccess::IDKeyAccess(QObject *parent): KeyAccess(parent) {
}

IDKeyAccess::~IDKeyAccess() {
}

KeyAccess::WriteKey *IDKeyAccess::checkout(int id) {
  WriteKey *key = KeyAccess::checkout();
  key2id.insert(key, id);
  id2key.insert(id, key);
  return key;
}

KeyAccess::WriteKey * IDKeyAccess::checkoutAll() {
  WriteKey *key = KeyAccess::checkout();
  key4all.insert(key);
  return key;
}

void IDKeyAccess::checkin(WriteKey *key) {
  KeyAccess::checkin(key);
  // Note: from here on out, key is destructed, but the value of the
  // pointer is still useful to index the maps.
  if (key4all.contains(key)) {
    key4all.remove(key);
    if (key4all.isEmpty())
      emit newAll();
  } else if (key2id.contains(key)) {
    int id = key2id[key];
    key2id.remove(key);
    QMultiMap<int, WriteKey *>::iterator i = id2key.find(id, key);
    if (i!=id2key.end())
      id2key.erase(i);
    // for some reason, the simple "id2key.remove(id, key)" fails in Qt 4.5.2
    if (id2key.find(id)==id2key.end())
      emit newDatum(id);
  }
}

void IDKeyAccess::verifyIDKey(KeyAccess::WriteKey *key, int id,
			      QString msg) const {
  verifyKey(key, msg);
  if (key2id[key]!=id)
    throw Exception("IDKeyAccess", "Key verification failed: " + msg);
}

void IDKeyAccess::verifyAllKey(KeyAccess::WriteKey *key,
			      QString msg) const {
  verifyKey(key, msg);
  if (!key4all.contains(key))
    throw Exception("IDKeyAccess", "Key verification failed: " + msg);
}

int IDKeyAccess::getID(WriteKey *key) const {
  if (key2id.contains(key))
    return key2id[key];
  else
    throw Exception("IDKeyAccess", "ID retrieval failed: unknown key");
}

IDKeyGuard::IDKeyGuard(IDKeyAccess &src, int id):
  src(src) {
  key_ = src.checkout(id);
}

IDKeyGuard::~IDKeyGuard() {
  src.checkin(key_);
}

AllKeyGuard::AllKeyGuard(IDKeyAccess &src):
  src(src) {
  key_ = src.checkoutAll();
}

AllKeyGuard::~AllKeyGuard() {
  src.checkin(key_);
}
