// floatimagetest.cpp

#include "floatimage.h"
#include <QImage>
#include <math.h>

int main() {
  QImage img("../../testdata/025.png");
  FloatImage fi(img);
  fi.noisify();
  FloatImage g = FloatImage::gaussian(10, 25);
  //  fi.apply(sqrtf);
  FloatImage avg = fi.convNorm(g);

  FloatImage dif = fi; dif -= avg;

  FloatImage f2 = dif;
  f2 *= f2;
  FloatImage c2 = f2.convNorm(g);
  c2.apply(sqrtf);
  FloatImage rms = c2.convNorm(g);

  rms += rms.mean()/4; // protect corners

  FloatImage ace = dif; ace /= rms;
  
  QImage i0(ace.toImage(-2, 2));
  i0.save("/tmp/i0.png");
  return 0;
}


