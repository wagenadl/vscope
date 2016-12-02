// pvpAccess.h

#ifndef PVPACCESS_H

#define PVPACCESS_H

#include <pvp/pvpEnumInt.h>

class pvpAccess {
public:
  enum AccessV {
    Error=0,
    ReadOnly,
    ReadWrite,
    ExistCheckOnly,
    WriteOnly };
  pvpAccess(AccessV x): x(x) {}
  pvpAccess(int x0=0) { x=AccessV(x0); }
  operator pvpEnumInt() const { return x; }
  char const *decode() const;
private:
  AccessV x;
};

#endif
