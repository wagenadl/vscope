#include <stdio.h>

#include <math/fft.h>

int main(int, char **) {
  rvector x; x.resize(1024);
  for (int n=0; n<1024; n++) {
    x[n] = 1 + cos(2*M_PI*n/1024)  + sin(4*M_PI*n/1024)
      + .5*cos(10*M_PI*n/1024) + .5*sin(20*M_PI*n/1024) + .125*cos(1024*M_PI*n/1024);
  }
  cvector z;
  copy(x,z);
  for (int i=0; i<10; i++) 
    printf("%4i: %6.3f + i*%6.3f\n",i,z[i].re,z[i].im);
  cvector f;
  cmul(z,1,f);
  FFT_insitu(f);
  for (int i=0; i<1024; i++) 
    printf("%4i: %6.3f + i*%6.3f\n",i,f[i].re,f[i].im);
  return 0;
}

