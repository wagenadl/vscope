// keyaccess.cpp

#include "keyaccess.h"
#include <base/dbg.h>
#include <base/exception.h>

KeyAccess::KeyAccess(QObject *parent): QObject(parent) {
  mustEmit = false;
}

KeyAccess::~KeyAccess() {
  if (checkedOut())
    Dbg()
      << "KeyAccess destroyed while keys still checked out. Disaster imminent.";
}

class KeyAccess::WriteKey {
};

bool KeyAccess::checkedOut() const {
  return !keys.isEmpty();
}

bool KeyAccess::checkedOut(KeyAccess::WriteKey *key) const {
  return keys.contains(key);
}

KeyAccess::WriteKey *KeyAccess::checkout() {
  WriteKey *key = new WriteKey();
  keys.insert(key);
  return key;
}

void KeyAccess::verifyKey(KeyAccess::WriteKey *key, QString msg) const {
  if (!keys.contains(key))
    throw Exception(myName(),
		    msg.isEmpty() ? "Key verification failed" : msg);
}

void KeyAccess::cancel(KeyAccess::WriteKey *key) {
  if (!keys.contains(key))
    throw Exception(myName(), "Canceling with unknown key");
  keys.remove(key);
  delete key;
  if (mustEmit)
    emitUnlessCheckedOut();
}

void KeyAccess::checkin(KeyAccess::WriteKey *key) {
  if (!keys.contains(key))
    throw Exception(myName(), "Checking in with unknown key");
  keys.remove(key);
  delete key;
  emitUnlessCheckedOut();
}

void KeyAccess::emitUnlessCheckedOut() {
  if (keys.isEmpty()) {
    mustEmit = false;
    emit newData();
  } else {
    mustEmit = true;
  }
}

QString KeyAccess::myName() const {
  QString obj = objectName();
  if (obj.isEmpty())
    return metaObject()->className() + QString("@")
      + QString::number((long int)(void*)this);
  else
    return obj;
}

KeyGuard::KeyGuard(KeyAccess &src): src(src) {
  key_ = src.checkout();
}

KeyGuard::~KeyGuard() {
  if (key_) 
    src.checkin(key_);
}

void KeyGuard::cancel() {
  if (key_)
    src.cancel(key_);
  key_ = 0;
}
