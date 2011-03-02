// cohdata.h

#ifndef COHDATA_H

#define COHDATA_H

#include <base/roidata.h>
#include <math/cvec.h>
#include <QSet>
#include <QMap>
#include <base/ccdtiming.h>
#include <base/campair.h>

class CohData: public QObject {
  Q_OBJECT;
public:
  enum { refANALOG, refDIGITAL, refFIXED } refType;
public:
  CohData();
  virtual ~CohData();
private:
  CohData(CohData const &other);
  CohData &operator=(CohData const &other);
public slots:
  void newROISet(class ROISet const *roiset);
  void newEPhys(class AnalogData const *ad, class DigitalData const *dd);
  void newCCDData(class ROIData3Set *rs3d);
  void setRefTrace(int achn);
  void setRefDigi(int digiline);
  void setRefFreq(double fref_hz);
  double getFStarHz() const { return fstar_hz; }
  QMap<int, double> const &magnitudes(); // this will validate if needed
  QMap<int, double> const &phases();
  double magnitude(int id); // this will validate if needed
  double phase(int id);
  void invalidate();
  class ROIData3Set *currentData() const { return rs3d; }
signals:
  void newData();
private:
  bool validate();
  void recalcReference();
  void refineTiming();
private:
  void copy(CohData const &other);
private:
  class ROISet const *roiset; // we do not own this
  class ROIData3Set *rs3d; // we do not own this
  class AnalogData const *adata; // we do not own this
  class DigitalData const *ddata; // we do not own this
private:
  int ref_chn; // doubles as ref_digiline
  double ref_hz; // for refFIXED mode
  QMap<int, double> coh_mag;
  QMap<int, double> coh_pha;
  QMap<CamPair, CCDTiming> timing;
  class CohEst *cohest;
  class PSDEst *psdest;
  QMap<CamPair, rvec> refs;
  double fstar_hz;
  QSet<QString> warned; // set of missing taper ids that we have warned about
  bool valid;
};

#endif
