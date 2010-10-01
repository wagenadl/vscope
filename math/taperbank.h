// taperbank.h

#ifndef TAPERBANK_H

#define TAPERBANK_H

#include <math/taperid.h>
#include <math/tapers.h>
#include <QString>
#include <QHash>

class Taperbank {
public:
  Taperbank(QString dir);
  ~Taperbank();
  void forget(TaperID const &id);
  void destroy(TaperID const &id); // delete from disk
  bool contains(TaperID const &id);
  Tapers const &find(TaperID const &id);
public:
  static void initialize(QString dir);
  static bool hasTaper(TaperID const &id);
  static Tapers const &tapers(TaperID const &id);
private:
  QString buildfn(TaperID const &id) const;
private:
  QHash<TaperID, Tapers> data;
  QString dir;
  static Taperbank *deflt;
};

#endif
