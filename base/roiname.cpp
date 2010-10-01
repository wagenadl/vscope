// roiname.cpp

#include <base/roiname.h>

#include <string.h>
#include <stdio.h>

ROIName::ROIName(int n) {
  name[0]='a';
  name[1]=0;
  for (int i=0; i<n; i++)
    step();
}

ROIName ROIName::next() const {
  ROIName x = *this;
  x.step();
  return x;
}

bool ROIName::step() {
  int l = strlen(name);
  for (int k=l-1; k>=0; k--) {
    name[k]++;
    if (name[k]<='z')
      return true;
    name[k]='a';
  }
  // if we're here, we overstepped
  if (l>=ROIName_STRLEN-1) {
    // This number got too big
    fprintf(stderr,"ROIName: Warning: number too big to represent base26.\n");
    return false;
  }
  for (int k=l; k>0; k--)
    name[k] = name[k-1];
  name[0]='a';

  return true;
}
