// ccdtrigmode.cpp

#include "ccdtrigmode.h"

CCDTrigMode::CCDTrigMode(CCDTrigMode::V x): x(x) {
}

char const *CCDTrigMode::toString() const {
  switch (x) {
  case Immediate: return "Immediate";
  case FirstFrame: return "FirstFrame";
  case EachFrame: return "EachFrame";
  case Bulb: return "Bulb";
  default: return "UNKNOWN";
  }
}
