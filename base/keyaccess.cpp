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

