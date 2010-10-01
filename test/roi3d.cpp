// roi3d.cpp - test of roi3data

#include <base/ccddata.h>
#include <base/roidata.h>
#include <base/roi3data.h>
#include <math.h>

int main() {
  CCDData *cc = new CCDData(64,64,100);
  CCDData *ox = new CCDData(64,64,100);
  for (int f=0; f<100; f++) {
    uint16_t *ccdat = cc->frameData(f);
    uint16_t *oxdat = ox->frameData(f);
    uint16_t ccval = uint16_t(5000+2000*cos(2*3.1415*f/10) - 10*f);
    uint16_t oxval = uint16_t(4000-1000*cos(2*3.1415*f/10) - 13*f);
    
    for (int p=0; p<64; p++) {
      for (int s=0; s<64; s++) {
	*ccdat++=ccval;
	*oxdat++=oxval;
      }
    }
  }

  ROI3Data r3d;
  r3d.setData(cc,ox);
  r3d.setROI(XYRRA(32,32,16,16,0));
  r3d.setDebleach(ROIData::None);

  int N = r3d.getNFrames();
  double const *ccdat = r3d.dataDonor();
  double const *oxdat = r3d.dataAcceptor();
  double const *ratdat = r3d.dataRatio();

  for (int n=0; n<N; n++) 
    printf("%8.1f %8.1f %8.1f\n",
	   *ccdat++,*oxdat++,*ratdat++);

  return 0;
}
