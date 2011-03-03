// ptrguard.h

#ifndef PTRGUARD_H

#define PTRGUARD_H

#include <base/dbg.h>

template <class X> class PtrGuard {
public:
  PtrGuard(X *x): x(x)  { dbg("PtrGuard(%p)=%p",this,x); }
  ~PtrGuard() { dbg("~PtrGuard(%p)=%p",this,x); delete x; }
  operator X*() { return x; }
private:
  X *x;
};

#endif
