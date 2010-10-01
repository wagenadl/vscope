#include <base/xyrra.h>
#include <base/solvep.h>
#include <stdlib.h>

#define K 12

void test_elfindnearest() {
  printf("clear\n");
  for (int k=0; k<K; k++) {
    double x0 = 300.*rand()/RAND_MAX;
    double y0 = 300.*rand()/RAND_MAX;
    double R = 100+200.*rand()/RAND_MAX;
    double r = R*(.3+.7*rand()/RAND_MAX);
    double a = 6.28*rand()/RAND_MAX;
    XYRRA xyrra(x0,y0,R,r,a);
    double x1 = 300.*rand()/RAND_MAX-150;
    double y1 = 300.*rand()/RAND_MAX-150;
    printf("xyrra{%i} = [ %g %g %g %g %g ]; ",k+1,x0,y0,R,r,a);
    printf("xy{%i} = [ %g %g ]; ",k+1,x0+x1,y0-y1);
    double omega = xyrra.findNearest(x0+x1,y0+y1);
    printf("omega{%i} = %g;\n",k+1,omega);
  }
  printf("ellipse;\n");
  //  printf("for k=1:%i, fprintf(1,'C: %%.3f. matlab: %%.3f\\n',omega{k},elfind_nearest(elbuild_xyrra(xyrra{k}),xy{k})); end\n",K);
}

void test_solvep4() {
  printf("clear\n");
  for (int k=0; k<K; k++) {
    double a = 300.*rand()/RAND_MAX-150;
    double b = 300.*rand()/RAND_MAX-150;
    double c = 300.*rand()/RAND_MAX-150;
    double d = 300.*rand()/RAND_MAX-150;
    double e = 300.*rand()/RAND_MAX-150;
    QList<double> xx = solvep4(a,b,c,d,e);
    printf("aa{%i} = %g; bb{%i} = %g; cc{%i} = %g; dd{%i} = %g; ee{%i} = %g;",k+1,a,k+1,b,k+1,c,k+1,d,k+1,e);
    printf("xx{%i} = [ ",k+1);
    for (QList<double>::iterator i=xx.begin(); i!=xx.end(); ++i) {
      printf("%g ",*i);
    }
    printf("];\n");
  }
  printf("for k=1:%i, fprintf(1,'C: ['); fprintf('%%.3f ',xx{k}); fprintf(1,']. matlab: ['); x=solvep4(aa{k},bb{k},cc{k},dd{k},ee{k}); x=x(abs(imag(x))<abs(real(x))); fprintf(1,'%%.3f ',x); fprintf(1,'].\\n'); end\n",K);
}

int main() {
  test_elfindnearest();
  //  test_solvep2();
  //  test_solvep4();
  return 0;
}
