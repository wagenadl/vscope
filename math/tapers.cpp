// tapers.cpp

#include "tapers.h"

Tapers::Tapers() {
  length = 0;
  nTapers = 0;
}

Tapers::~Tapers() {
  rebuild(0,0);
}

void Tapers::rebuild(int len, int nTap) {
  if (nTap!=nTapers)
    tapers.resize(nTap);

  for (int k=0; k<nTap; k++) 
    if (tapers[k].size() != len)
      tapers[k].resize(len);
  
  length=len;
  nTapers=nTap;
}
