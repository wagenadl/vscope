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
#include <base/enums.h>
#include <base/stimulusdef.h>

class CohData: public KeyAccess {
  Q_OBJECT;
public:
  CohData();
  virtual ~CohData();
public slots:
  void setROISet(class ROISet const *roiset);
  void setEPhys(class AnalogData const *ad,
		class DigitalData const *dd);
  void setCCDData(class ROIData3Set *rs3d);
  void setRefTrace(QString achn, bool train=false);
  void setRefTrain(StimulusDef const &s);
  void setRefDigi(QString digiline);
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
  static void filterForTrain(rvec &ref, double fs_hz);
  static void detrend(rvec &ref);
  static void crazyFilter(rvec &psd, double df_hz);
private:
  void copy(CohData const &other);
private:
  class ROISet const *roiset; // we do not own this
  class ROIData3Set *rs3d; // we do not own this
  class AnalogData const *adata; // we do not own this
  class DigitalData const *ddata; // we do not own this
private:
  REFTYPE refType;
  struct { // I'd like to say union, but that's not OK in C++.
    QString chn; // for RT_Analog, and RT_Digital mode
    double freq_Hz; // for RT_Frequency mode
    StimulusDef stim; // for RT_Train
  } refID;
  struct Data {
    QMap<int, double> coh_mag;
    QMap<int, double> coh_pha;
    QHash<CamPair, CCDTiming> timing;
    QHash<CamPair, rvec> refs;
    QHash<CamPair, double> fstar_hz;
    QHash<CamPair, TaperID> taperIDs;
    bool valid;
  };
  mutable Data data;
  class CohEst *cohest;
  class PSDEst *psdest;
private:
  static QSet<QString> warned;
  /*:V warned
   *:D Set of missing taper ids that we have already warned the user about.
   */
private:
  CohData(CohData const &other);
  CohData &operator=(CohData const &other);
};

#endif
