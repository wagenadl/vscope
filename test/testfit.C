#include <stdio.h>
#include <math.h>

double fitpeak(double *yy, int x0) {
  /* Fits a quadratic to the data yy, which is sampled at x=-x0..+x0,
     and use it to estimate the location of the peak in yy.
     If f(x) = ax^2 + bx + c, then
     chi2 = sum_x (f(x)-y(x))^2,
     so dchi2/da = 2 sum_x x^2 * (f(x)-y(x)) == 0
        dchi2/db = 2 sum_x x * (f(x)-y(x)) == 0
        dchi2/dc = 2 sum_x (f(x)-y(x)) == 0
     i.e.
       sum_x ax^4 + bx^3 + cx^2 - y(x) x^2 = 0
       sum_x ax^3 + bx^2 + cx   - y(x) x = 0
       sum_x ax^2 + bx   + c    - y(x) = 0
     i.e.
       X4 a + X3 b + X2 c = YX2
       X3 a + X2 b + X1 c = YX1
       X2 a + X1 b + X0 c = YX0
     where Xn := sum_x x^n
     and YXn := sum_x y(x) x^n.
     Eliminate c from second-third:
       (X0 X3 - X1 X2) a + (X0 X2 - X1 X1) b = X0 YX1 - X1 YX0
     Write that as
             p3        a +        p2       b =        pp1
     Eliminate b from that-first:
       (X3 p3 - p2 X4) a = X3 pp1 - p2 YX2
     Solve for a:
       a = (X3 pp1 - p2 YX2) / (X3 p3 - p2 X4)
     Find b:
       b = (pp1 - p3 a) / p2
     And finally:
       x_peak = -b/2a.

     WARNING: if xx is symmetric, then X1:=0 and X3:=0, hence p3:=0,
     so instead I should work from:
       b = pp1/p2. ??
  */
  double X0=0;
  double X1=0;
  double X2=0;
  double X3=0;
  double X4=0;
  double YX0=0;
  double YX1=0;
  double YX2=0;
  for (int x=-x0; x<=x0; x++) {
    X0++;
    X1+=x;
    double x2=x*x;
    X2+=x2;
    double x3=x2*x;
    X3+=x3;
    double x4=x2*x2;
    X4+=x4;
    double y = yy[x+x0];
    YX0+=y;
    YX1+=y*x;
    YX2+=y*x2;
  }
  double p3 = X0*X3-X1*X2;
  double p2 = X0*X2-X1*X1;
  double pp1 = X0*YX1-X1*YX0;
  double a = (X3*pp1-p2*YX2) / (X3*p3-p2*X4);
  double b = (pp1-p3*a) / p2;
  // should we issue a warning if a>0?
  printf("  X0=%g X1=%g X2=%g X3=%g X4=%g\n",X0,X1,X2,X3,X4);
  printf("  YX0=%g YX1=%g YX2=%g\n",YX0,YX1,YX2);
  printf("  p3=%g p2=%g pp1=%g\n",p3,p2,pp1);
  printf("  a=%g b=%g\n",a,b);
  return -b/(2*a);
}

int main(int, char **) {
  double yy[3];
  double xx0[] = {0,0,0,   1,1,1,    2,2,2};
  double aa[] = {-2,-2,-1, -2,-2,-1, -2,-2,-1};
  double cc[] = { 0,1,1,   0,1,1,    0,1,1 };
  for (int k=0; k<9; k++) {
    for (int dx=-1; dx<=1; dx++) 
      yy[dx+1] = aa[k]*(dx-xx0[k])*(dx-xx0[k]) + cc[k];
    double xpk = fitpeak(yy,1);
    printf("a=%g c=%g x0=%g -> %g\n\n",aa[k],cc[k],xx0[k],xpk);
  }
  return 0;
}

	 
