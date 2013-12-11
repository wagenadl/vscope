// campair.h

#ifndef CAMPAIR_H

#define CAMPAIR_H

#include <QString>
#include <QHash>

class CamPair {
public:
  CamPair() { donor = acceptor = ""; }
public:
  QString donor; // or single camera if not a pair
  QString acceptor; // or empty
  bool operator==(CamPair const &other) const {
    return donor==other.donor && acceptor==other.acceptor;
  }
  bool operator!=(CamPair const &other) const {
    return !operator==(other);
  }
};

inline uint qHash(CamPair const &cp) {
  return qHash(cp.donor) + ~qHash(cp.acceptor);
}

#endif
