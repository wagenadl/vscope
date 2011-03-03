// cohdata.h

#ifndef COHDATA_H

#define COHDATA_H

#include <base/roidata.h>
#include <math/cvec.h>
#include <QSet>
#include <QMap>
#include <QHash>
#include <base/ccdtiming.h>
#include <base/campair.h>
#include <math/taperid.h>

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
  void setFrameLine(QString camera, int line);
  void setROISet(class ROISet const *roiset);
  void setEPhys(class AnalogData const *ad,
		class DigitalData const *dd);
  void setCCDData(class ROIData3Set *rs3d);
  void setRefTrace(int achn);
  void setRefDigi(int digiline);
  void setRefFreq(double fref_hz);
  void invalidate();
private slots:
  void updateROIs();
public: // these will validate if needed
  double getFStarHz(int id);
  double getTypicalFStarHz();
  QMap<int, double> const &magnitudes();
  QMap<int, double> const &phases();
  double magnitude(int id);
  double phase(int id);
public:
  class ROIData3Set *currentData() const { return rs3d; }
signals:
  void newData();
private:
  bool validate();
  void recalcReference();
  void recalcTiming();
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
  QHash<CamPair, CCDTiming> timing;
  class CohEst *cohest;
  class PSDEst *psdest;
  QHash<CamPair, TaperID> taperIDs;
  QHash<CamPair, rvec> refs;
  QHash<CamPair, double> fstar_hz;
  QSet<QString> warned; // set of missing taper ids that we have warned about
  QHash<QString, int> digilines;
  bool valid;
};

#endif
