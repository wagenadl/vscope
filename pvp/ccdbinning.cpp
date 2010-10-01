// ccdbinning.cpp

#include "ccdbinning.h"


CCDBinning::CCDBinning() {
  sbin = pbin = 1;
}

CCDBinning::CCDBinning(int bin) {
  sbin = pbin = bin;
}

CCDBinning::CCDBinning(int sbin_, int pbin_) {
  sbin = sbin_;
  pbin = pbin_;
}
