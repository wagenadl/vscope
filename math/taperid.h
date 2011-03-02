// taperid.h

#ifndef TAPERID_H

#define TAPERID_H

#include <QString>

class TaperID {
public:
  TaperID(int length=0, double nw=4);
  TaperID(int length, double dt, double df);
  double freqRes(double dt) const;
  bool operator==(TaperID const &b) const;
  QString name() const;
public:
  int N;
  double nw;
  int K;
};

extern uint qHash(TaperID const &tid);

#endif
