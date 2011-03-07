// keyaccess.h

#ifndef KEYACCESS_H

#define KEYACCESS_H

#include <QObject>
#include <QSet>

class KeyAccess: public QObject {
  Q_OBJECT;
public:
  KeyAccess(QObject *parent=0);
  virtual ~KeyAccess();
  class WriteKey;
  /*:C WriteKey
   *:D Opaque data type to handle access permissions.
   */
  virtual WriteKey *checkout();
  virtual void checkin(WriteKey *);
  /*:F checkout, cancel
   *:D To gain write access to the data, you must request a key using
       checkout(). Once you're done writing, call checkin() with the same
       key.
       newData() is emitted upon final checkin.
   *:N Do *not* delete keys yourself. Call checkin.
   */
  void makeExclusive(bool exclusive=true);
  /*:F makeExclusive
   *:D Turns this access into exclusive or shared
   *:N Making the access exclusive while multiple keys are checked out
       causes an exception.
  */
signals:
  void newData();
  /*:S newData
   *:D Emitted upon checkin or by emitUnlessCheckedOut().
   */
protected:
  void emitUnlessCheckedOut();
  /*:F emitUnlessCheckedOut
   *:D Emits the newData() signal unless any key is checked out.
   */
  bool checkedOut(WriteKey *key) const;
  bool checkedOut() const;
  /*:F checkedOut
   *:D Returns true if the given key (or any key) is checked out.
   */
  void verifyKey(WriteKey *key, QString msg="") const;
  /*:F verifyKey
   *:D Checks if the given key is currently checked out.
   */
private:
  QString myName() const;
private:
  QSet<WriteKey *> keys;
  bool exclusive;
};

class KeyGuard {
  /*:C KeyGuard
   *:D Convenience class for writing the data in a KeyAccess object.
       Instead of writing:
         KeyAccess::WriteKey *key = source.checkout();
	 Data *data = source.data(key); // or whatever
	 // write to data
	 source.checkin(key);
       you can write
         KeyGuard guard(source);
	 Data *data = source.data(guard.key());
	 // write to data
       That way, checkin is automatic when guard goes out of scope.
  */
public:
  KeyGuard(KeyAccess &src);
  ~KeyGuard();
  /*:F destructor
   *:D Checks in the key.
   */
  KeyAccess::WriteKey *key() const { return key_; }
private:
  KeyAccess &src;
  KeyAccess::WriteKey *key_;
};

#endif
