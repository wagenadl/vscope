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
#include <base/keyaccess.h>

class CohData: public KeyAccess {
  Q_OBJECT;
public:
  enum { refANALOG, refDIGITAL, refFIXED } refType;
public:
  CohData();
  virtual ~CohData();
public slots:
  void setFrameLine(QString camera, int line);
  void setROISet(class ROISet const *roiset);
  void setEPhys(class AnalogData const *ad,
		class DigitalData const *dd);
  void setCCDData(class ROIData3Set *rs3d);
  void setRefTrace(QString achn);
  void setRefDigi(int digiline);
  void setRefFreq(double fref_hz);
  void invalidate();
private slots:
  void updateROIs();
public: // these will validate if needed
  double getFStarHz(int id) const;
  double getTypicalFStarHz() const;
  QMap<int, double> const &magnitudes() const;
  QMap<int, double> const &phases() const;
  double magnitude(int id) const;
  double phase(int id) const;
public:
  class ROIData3Set *currentData() const { return rs3d; }
private:
  bool validate() const;
  void recalcReference() const;
  void recalcTiming() const;
private:
  void copy(CohData const &other);
private:
  class ROISet const *roiset; // we do not own this
  class ROIData3Set *rs3d; // we do not own this
  class AnalogData const *adata; // we do not own this
  class DigitalData const *ddata; // we do not own this
private:
  QString ref_chn; // for refANALOG mode
  int ref_line; // for refDIGITAL mode
  double ref_hz; // for refFIXED mode
  struct Data {
    QMap<int, double> coh_mag;
    QMap<int, double> coh_pha;
    QHash<CamPair, CCDTiming> timing;
    bool valid;
  };
  mutable Data data;
  class CohEst *cohest;
  class PSDEst *psdest;
  QHash<CamPair, TaperID> taperIDs;
  QHash<CamPair, rvec> refs;
  QHash<CamPair, double> fstar_hz;
  QSet<QString> warned; // set of missing taper ids that we have warned about
  QHash<QString, int> digilines;
private:
  CohData(CohData const &other);
  CohData &operator=(CohData const &other);
};

#endif
