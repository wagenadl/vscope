// fft.cpp

#include "fft.h"
#include <base/exception.h>

#define FFT_MAKECASE(n) case 1<<n: { \
    DanielsonLanczos<1<<n> dl; \
    dl.apply(dat); \
  } break;

namespace FFT {
  inline void swap(double &a, double &b) {
    double z = a;
    a = b;
    b = z;
  }
  void scramble(Cplx *x, int nn) {
    // reverse-binary reindexing
    double *me = *x;
    int n = 2*nn;
    int j=1;
    for (int i=1; i<n; i+=2) {
      if (j>i) {
	swap(me[j-1], me[i-1]);
	swap(me[j], me[i]);
      }
      int m = nn;
      while (m>=2 && j>m) {
	j -= m; m >>= 1;
      }
      j += m;
    };
  }

  void FFT(Cplx *y, int n) {
    scramble(y,n);
    double *dat = *y;
    switch (n) {
      FFT_MAKECASE(0);
      FFT_MAKECASE(1);
      FFT_MAKECASE(2);
      FFT_MAKECASE(3);
      FFT_MAKECASE(4);
      FFT_MAKECASE(5);
      FFT_MAKECASE(6);
      FFT_MAKECASE(7);
      FFT_MAKECASE(8);
      FFT_MAKECASE(9);
      FFT_MAKECASE(10);
      FFT_MAKECASE(11);
      FFT_MAKECASE(12);
      FFT_MAKECASE(13);
      FFT_MAKECASE(14);
      FFT_MAKECASE(15);
      FFT_MAKECASE(16);
      FFT_MAKECASE(17);
      FFT_MAKECASE(18);
      FFT_MAKECASE(19);
      FFT_MAKECASE(20);
      FFT_MAKECASE(21);
      FFT_MAKECASE(22);
      FFT_MAKECASE(23);
      FFT_MAKECASE(24);
    default:
      throw Exception("FFT","Unknown size for FFT. Currently, fft.cpp only defines FFT for powers of two up to 2^24.");
    }
  };
};

void FFT_insitu(cvec &x) {
  FFT::FFT(x.data(),x.size());
}
