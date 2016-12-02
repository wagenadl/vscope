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
