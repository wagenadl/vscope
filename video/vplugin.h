// Plugin.H

#ifndef PLUGIN_H

#define PLUGIN_H

#include <stdint.h>
#include <QString>

class VPRenderInfo {
public:
  double *destx;
  double *desty;
  uint32_t *destd;
  uint32_t dmask_light; // bit mask to switch on light
  int nscans;
  int nchans; // i.e. step size for destx, desty. Does not apply to destd.
  double fs_hz; // sampling rate

  int i0; // first scan to fill with a frame
  int nfill; // number of scans to fill for each frame
  int frameival; // start-to-start frame period
  int nframes; // number of frames to produce
  /* VideoCommPlugins guarantees that all frames are within 
     the destination data buffer. */
};

class VPlugin {
public:
  VPlugin();
  virtual ~VPlugin();
public:
  void setscale(double xscale=1, double yscale=1);
  void sethome(double xhome=2, double yhome=2);
  virtual QString name()=0;
  virtual QString parnames()=0;
  virtual QString parunits()=0;
  virtual QString description()=0;
  virtual QString parsuggest(unsigned int parno); // parno=1,2,3
  virtual bool setpar(unsigned int parno, double value); // parno=1,2,3
  virtual bool checkpars();
  virtual void render(VPRenderInfo const &info) const;
  /* The base class renderer turns on the light during each frame. Derived
     classes may switch light off for parts of each frame. For this scheme
     to work, derived classes *must* call the base class's render method.
   * VideoCommPlugins will take care of pre-illumination. */
protected:
  double xscale;
  double yscale;
  double xhome;
  double yhome;
};

#endif
