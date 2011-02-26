// campair.h

#ifndef CAMPAIR_H

#define CAMPAIR_H

#include <QString>

class CamPair {
public:
  CamPair() { donor = acceptor = ""; }
public:
  QString donor; // or single camera if not a pair
  QString acceptor; // or empty
  bool operator==(CamPair const &other) const {
    return donor==other.donor && acceptor==other.acceptor;
  }
};

#endif
