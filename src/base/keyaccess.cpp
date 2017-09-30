// base/keyaccess.cpp - This file is part of VScope.
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

// keyaccess.cpp

#include "keyaccess.h"
#include <base/dbg.h>
#include <base/exception.h>

KeyAccess::KeyAccess(QObject *parent): QObject(parent) {
  exclusive = false;
}

KeyAccess::~KeyAccess() {
  if (anyCheckedOut())
    Dbg()
      << "KeyAccess destroyed while keys still checked out. Disaster imminent.";
}

class KeyAccess::WriteKey {
};

bool KeyAccess::anyCheckedOut() const {
  return !keys.isEmpty();
}

bool KeyAccess::isCheckedOut(KeyAccess::WriteKey *key) const {
  return keys.contains(key);
}

KeyAccess::WriteKey *KeyAccess::checkout() {
  if (exclusive && !keys.isEmpty())
    throw Exception(myName(), "Requested second checkout though exclusive");
  WriteKey *key = new WriteKey();
  keys.insert(key);
  return key;
}

void KeyAccess::verifyKey(KeyAccess::WriteKey *key, QString msg) const {
  if (!keys.contains(key))
    throw Exception(myName(),
		    msg.isEmpty() ? "Key verification failed" : msg);
}

void KeyAccess::checkin(KeyAccess::WriteKey *key) {
  if (!keys.contains(key))
    throw Exception(myName(), "Checking in with unknown key");
  keys.remove(key);
  delete key;
  emitUnlessCheckedOut();
}

void KeyAccess::emitUnlessCheckedOut() {
  //Dbg(this) << "KeyAccess::emitUnlessCheckedOut";
  if (keys.isEmpty()) {
    //Dbg(this) << "emitting newData";
    emit newData();
  }
}

void KeyAccess::makeExclusive(bool e) {
  exclusive = e;
  if (exclusive && keys.size()>1)
    throw Exception(myName(),
		    "Cannot make exclusive: multiple keys checked out");
}

QString KeyAccess::myName() const {
  QString obj = objectName();
  if (obj.isEmpty())
    return metaObject()->className() + QString("@0x")
      + QString::number((long int)(void*)this, 16);
  else
    return obj;
}

KeyGuard::KeyGuard(KeyAccess &src): src(src) {
  key_ = src.checkout();
}

KeyGuard::~KeyGuard() {
  src.checkin(key_);
}

