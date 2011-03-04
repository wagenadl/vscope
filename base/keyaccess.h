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
  WriteKey *checkout();
  void checkin(WriteKey *);
  void cancel(WriteKey *);
  /*:F checkout, checkin, cancel
   *:D To gain write access to the frame data, you must request a key using
       checkout(). The contents of the key is opaque. Once you're done writing,
       call checkin() with the same key. If you decide not to write, call
       cancel().
       newData() is emitted upon final checkin.
   *:N Slightly counterintuitively, if two clients check out, and one checks
       in first after which the other cancels, a signal is emitted at the
       time the second client cancels.
   *:N Do *not* delete keys yourself. Call either checkin or cancel.
   */
  bool makeExclusive(bool exclusive=true);
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
  bool checkedOut(WriteKey *key) const;
  bool checkedOut() const;
  /*:F checkedOut
   *:D Returns true if the given key (or any key) is checked out.
   */
  void verifyKey(WriteKey *key, QString msg="") const;
private:
  QString myName() const;
private:
  QSet<WriteKey *> keys;
  bool mustEmit;
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
   *:D Checks in the key unless cancel() has been called.
   */
  void cancel();
  /*:F cancel
   *:D Calls cancel() on the source of the key. The key becomes invalid.
   *:N Subsequent cancellations have no effect.
   */
  KeyAccess::WriteKey *key() { return key_; }
private:
  KeyAccess &src;
  KeyAccess::WriteKey *key_;
};

#endif
