// pvpAccess.cpp

#include "pvpAccess.h"


char const *pvpAccess::decode() const {
  switch (x) {
  case Error: return "Error";
  case ReadOnly: return "ReadOnly";
  case ReadWrite: return "ReadWrite";
  case ExistCheckOnly: return "ExistCheckOnly";
  case WriteOnly: return "WriteOnly";
  default: return "UNKNOWN";
  }
}
