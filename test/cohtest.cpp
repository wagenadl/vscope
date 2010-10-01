// cohtest.cpp

#include <math/cohest.h>
#include <math/cohestci.h>
#include <math/psdest.h>
#include <math/taperbank.h>
#include <stdio.h>
#include <base/dbg.h>
#include <base/exception.h>

int main() {
  try {
  unsigned int N=128;
  FILE *in = fopen("sig1.dat","rb");
  double *x = new double[N];
  if (fread(x, sizeof(double), N, in)!=N) {
    perror("cannot read sig1.dat");
    return 1;
  }
  fclose(in);
  
  in = fopen("sig2.dat","rb");
  double *y = new double[N];
  if (fread(y, sizeof(double), N, in)!=N) {
    perror("cannot read sig2.dat");
    return 1;
  }
  fclose(in);
  
  Taperbank tb("tapers");
  TaperID tid(128,10/3.0);
  if (!tb.hasTaper(tid)) 
    fprintf(stderr,"cohtest: I don't have the right taper\n");
  dbg("have tapers");
  Tapers const &tap(tb.tapers(tid));
  dbg("got tapers");
  if (!tb.hasTaper(tid)) 
    fprintf(stderr,"cohtest: I still don't have the right taper!??\n");

  rvec vx; vx.resize(N);
  for (unsigned int n=0; n<N; n++)
    vx[n]=x[n];
  rvec vy; vy.resize(N);
  for (unsigned int n=0; n<N; n++)
    vy[n]=y[n];
  dbg("copied vecs");

  fprintf(stderr,"Power spectral densities\n");
  PSDEst psdx;
  PSDEst psdy;
  psdx.compute(vx, 1/25., .65, tap);
  psdy.compute(vy, 1/25., .65, tap);
  int K=psdx.freqbase.size();
  for (int k=0; k<K; k++)
    fprintf(stderr,"%.4f %.4f %.4f\n",psdx.freqbase[k],psdx.psd[k],psdy.psd[k]);
  
  TaperID tidc(128,4.10);
  if (!tb.hasTaper(tidc)) 
    fprintf(stderr,"cohtest: I don't have the right taper\n");
  dbg("have tapers");
  Tapers const &tapc(tb.tapers(tidc));
  dbg("got tapers");
  if (!tb.hasTaper(tidc)) 
    fprintf(stderr,"cohtest: I still don't have the right taper!??\n");

  fprintf(stderr,"Coherence w/o conf. ival.\n");
  CohEst coh;
  coh.compute(vy,vx, 1/25., 4.1/(128*1/25.), tapc);
  for (int i=0; i<1000; i++) {
    coh.compute(vy,vx, 1/25., 4.1/(128*1/25.), tapc,4);
  }
  K=coh.freqbase.size();
  for (int k=0; k<K; k++)
    fprintf(stderr,"%.4f %.4f %.4f\n",coh.freqbase[k],coh.magnitude[k],coh.phase[k]);

  coh.compute(vy,vx, 1/25., 4.1/(128*1/25.), tapc, 4);
  K=coh.freqbase.size();
  for (int k=0; k<K; k++)
    fprintf(stderr,"%.4f %.4f %.4f\n",coh.freqbase[k],coh.magnitude[k],coh.phase[k]);

  fprintf(stderr,"Coherence with conf. ival.\n");
  CohEstCI cohci;
  double ci_factor = 1.0152;
	  
  cohci.compute(vy,vx, 1/25., 4.1/(128*1/25.), tapc, ci_factor);
  K=cohci.freqbase.size();
  for (int k=0; k<K; k++)
    fprintf(stderr,"%.4f %.4f %.4f %.4f %.4f %.4f %.4f\n",cohci.freqbase[k],
	    cohci.magnitude[k],cohci.magnitude_lo[k],cohci.magnitude_hi[k],
	    cohci.phase[k],cohci.phase_lo[k],cohci.phase_hi[k]);

  cohci.compute(vy,vx, 1/25., 4.1/(128*1/25.), tapc, ci_factor, 4);
  K=cohci.freqbase.size();
  for (int k=0; k<K; k++)
    fprintf(stderr,"%.4f %.4f %.4f %.4f %.4f %.4f %.4f\n",cohci.freqbase[k],
	    cohci.magnitude[k],cohci.magnitude_lo[k],cohci.magnitude_hi[k],
	    cohci.phase[k],cohci.phase_lo[k],cohci.phase_hi[k]);
	    
 
  return 0;
  } catch (Exception const &e) {
    e.report();
    return 2;
  }
}
