// roi3data.h

#ifndef ROI3DATA_H

#define ROI3DATA_H

#include <QObject>
#include <base/roicoords.h>
#include <base/roidata.h>

class ROI3Data_ {
protected:
  ROIData datDonor, datAcceptor;
  double *datRatio;
  ROIData::Debleach debleach;
  int nRatio;
  double t0Ratio_ms, dtRatio_ms;
  bool valid;
  static bool donorflipx, donorflipy;
  static bool acceptorflipx, acceptorflipy;
};

class ROI3Data: private ROI3Data_ {
  /*:C ROI3Data
   *:D ROI3Data contains the time-dependent average fluorescence in a ROI.
       It contains the data for the donor (e.g, coumarin) and the acceptor
       (e.g., oxonol) as well as the ratio of the two.
       The class is smart, in that setROI() and setData() do not actually
       cause computation of the trace; this is only done when the results
       are requested through dataDonor() and friends.
  */
public:
  ROI3Data();
  virtual ~ROI3Data();
  ROI3Data(ROI3Data const &other);
  ROI3Data &operator=(ROI3Data const &other);
  void setROI(ROICoords const *roi);
  /*:F setROI
   *:D Redefines which portion of the CCD image we care about.
       This does not cause recomputation and is thus trivially fast.
  */
  void setData(class CCDData const *donor, class CCDData const *acceptor);
  /*:F setData
   *:D Redefines what CCD images we are working with.
       This does not cause recomputation and is thus trivially fast.
   *:N It is OK for either data pointer to be zero. In that case, the "ratio"
       will simply equal the other pointer. If both are zero, the "ratio"
       data will always be zero.
  */
  static void setFlip(bool donorflipx, bool donorflipy,
	       bool acceptorflipx, bool acceptorflipy);
  /*:F setFlip
   *:D Specifies whether the data is flipped in the x- and or y-dimensions
       relative to the ROI.
   *:N This is a static function that applies to any ROI3Datas created in the
       future. It DOES NOT affect pre-existing ROI3Datas.
  */    
  void setDebleach(ROIData::Debleach d);
  /*:F setDebleach
   *:D Specifies how we debleach.
       This does not cause recomputation and is thus trivially fast.
   */
  int getRatioNFrames() const;
  double getRatioT0ms() const;
  double getRatioDTms() const;
  /*:F getRatioNFrames, getRatioT0ms, getRatioDTms
   *:D Timing information of the ratio trace.
   *:D If there are two cameras, then the ratio trace is the ratio of donor
       over acceptor, or actually the dF/F of the donor minusthe dF/F of the
       acceptor. If there is only one camera, then the ratio trace is just the
       dF/F of that camera.
   *:N There is no sign inversion even if the one camera is the acceptor.
   */
  int getAcceptorNFrames() const;
  double getAcceptorT0ms() const;
  double getAcceptorDTms() const;
  int getDonorNFrames() const;
  double getDonorT0ms() const;
  double getDonorDTms() const;
  Range timeRange() const;
  double const *dataDonor();
  /*:F dataDonor
   *:D Returns the debleached and normalized data dF/F of the donor
       in percents.
       This recomputes if necessary but returns cached results whenever
       possible.
   *:N The returned pointer is valid until the next call to getRaw or
       until this ROIData gets destructed.
  */       
  double const *dataAcceptor();
  /*:F dataAcceptor
   *:D Like dataDonor, but for the acceptor image sequence.
   */
  double const *dataRatio();
  /*:F dataRatio
   *:D Like dataAcceptor, but for the ratio donor/acceptor.
   *:N We do not calculate a true ratio, but assume that dF/F is small
       so that:
       
          F_d + dF_d     F_d     1+dF/F_d     F_d           dF_d     dF_a
         ------------ = ----- * ---------- ~ ----- * ( 1 + ------ - ------ )
          F_a + dF_a     F_a     1+dF/F_a     F_a            F_d      F_a

       is a fair approximation.
       I'm not sure this is a great idea if we are not debleaching, but
       it was the easiest way to code this. (Otherwise, when does debleaching
       get done exactly?)
   *:N If we don't have acceptor data, this returns donor data w/o ratioing.
  */
private:
  void copy(ROI3Data const &other);
};

#endif
