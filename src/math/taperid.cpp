// taperid.cpp

#include "taperid.h"
#include <base/minmax.h>

TaperID::TaperID(int N, double nw):
  N(N), nw(nw) {
  K = floori(2*nw - 1);
}

TaperID::TaperID(int len, double dt, double df):
  N(len),
  nw(len*dt*df) {
  K = floori(2*nw - 1);
}

bool TaperID::operator==(TaperID const &b) const {
  return N==b.N && nw==b.nw;
}

double TaperID::freqRes(double dt) const {
  return nw/(N*dt);
}

QString TaperID::name() const {
  return QString("%1-%2").arg(N).arg(nw,0,'f',2);
}

uint qHash(TaperID const &tid) {
  return uint(tid.N) + uint(172891*tid.nw);
}
