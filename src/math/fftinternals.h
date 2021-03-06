// math/fftinternals.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// fftinternals.h

#ifndef FFTINTERNALS_H

#define FFTINTERNALS_H

#include <base/numbers.h>

namespace FFT {
  /* All of the following cleverness taken from a Dr. Dobbs article:
     Volodymyr Myrnyy: "A Simple and Efficient FFT Implementation in C++"
     May 10, 2007 and May 25, 2007.
     http://www.ddj.com/cpp/199500857
  */


  template<unsigned M, unsigned N, unsigned B, unsigned A>
  struct SinCosSeries {
    static double value() {
      return 1-(A*Numbers::pi()/B)*(A*Numbers::pi()/B)/M/(M+1)
	*SinCosSeries<M+2,N,B,A>::value();
    }
  };

  template<unsigned N, unsigned B, unsigned A> struct SinCosSeries<N,N,B,A> {
    static double value() {
      return 1.;
    }
  };

  template<unsigned B, unsigned A, typename T=double> struct Sin;

  template<unsigned B, unsigned A> struct Sin<B,A,float> {
    static float value() {
      return (A*Numbers::pi()/B)*SinCosSeries<2,24,B,A>::value();
    }
  };

  template<unsigned B, unsigned A> struct Sin<B,A,double> {
    static double value() {
      return (A*Numbers::pi()/B)*SinCosSeries<2,34,B,A>::value();
    }
  };

  template<unsigned B, unsigned A, typename T=double> struct Cos;

  template<unsigned B, unsigned A> struct Cos<B,A,float> {
    static float value() {
      return SinCosSeries<1,23,B,A>::value();
    }
  };

  template<unsigned B, unsigned A> struct Cos<B,A,double> {
    static double value() {
      return SinCosSeries<1,33,B,A>::value();
    }
  };
  
  template<unsigned N, typename T=double> class DanielsonLanczos {
    DanielsonLanczos<N/2,T> next;
  public:
    void apply(T* data) {
      next.apply(data);
      next.apply(data+N);
      
      T wtemp,tempr,tempi,wr,wi,wpr,wpi;
      wtemp = Sin<N,1,T>::value();      // wtemp = sin(Numbers::pi()/N);
      wpr = -2.0*wtemp*wtemp;
      wpi = -Sin<N,2,T>::value(); // wpi = -sin(2*Numbers::pi()/N);
      wr = 1.0; wi = 0.0;
      for (unsigned i=0; i<N; i+=2) {
	tempr = data[i+N]*wr - data[i+N+1]*wi;
	tempi = data[i+N]*wi + data[i+N+1]*wr;
	data[i+N] = data[i]-tempr;
	data[i+N+1] = data[i+1]-tempi;
	data[i] += tempr;
	data[i+1] += tempi;
	
	wtemp = wr;
	wr += wr*wpr - wi*wpi;
	wi += wi*wpr + wtemp*wpi;
      }
    }
  };

  template<typename T> class DanielsonLanczos<1,T> {
  public:
    void apply(T*) { }
  };
};

#endif
