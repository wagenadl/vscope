// ptrguard.h

#ifndef PTRGUARD_H

#define PTRGUARD_H

#include <base/dbg.h>

template <class X> class PtrGuard {
public:
  PtrGuard(X *x): x(x)  { }
  ~PtrGuard() { delete x; }
  operator X*() { return x; }
  X *ptr() { return x; }
private:
  X *x;
};

#endif
