// idkeyaccess.h

#ifndef IDKEYACCESS_H

#define IDKEYACCESS_H

#include <base/keyaccess.h>
#include <QMultiMap>
#include <QMap>
#include <QSet>

typedef int ID;

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
  virtual WriteKey *checkout(ID);
  virtual WriteKey *checkoutAll();
  virtual void checkin(WriteKey *);
  /*:F checkin
   *:D If the key was checked out with an ID, we emit newDatum(ID).
       If the key was checked out with checkoutAll, we emit newAll().
       In both cases, we also emit newData().
   */
  void verifyIDKey(WriteKey *key, ID id, QString msg="") const;
  void verifyAllKey(WriteKey *key, QString msg="") const;
  ID getID(WriteKey *key) const;
  /*:F getID
   *:D Returns the ID associated with the given key.
   *:N Throws an acception if the key is unknown or was not created by
       checkout(ID).
   */
signals:
  void newDatum(ID id);
  void newAll();
private:
  QMap<WriteKey *, ID> key2id;
  QMultiMap<ID, WriteKey *> id2key;
  QSet<WriteKey *> key4all;
};

class IDKeyGuard {
public:
  IDKeyGuard(IDKeyAccess &src, ID id);
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
