// base/roi3data.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// roi3data.h

#ifndef ROI3DATA_H

#define ROI3DATA_H

#include <QObject>
#include <base/roicoords.h>
#include <base/roidata.h>

class ROI3Data {
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
  void setROI(ROICoords const *roi);
  /*:F setROI
   *:D Redefines which portion of the CCD image we care about.
       This does not cause recomputation and is thus trivially fast.
  */
  void setData(class CCDData const *donor,
	       class CCDData const *acceptor,
	       bool noupdate=false);
  /*:F setData
   *:D Redefines what CCD images we are working with.
       This does not cause recomputation and is thus trivially fast.
   *:N It is OK for either data pointer to be zero. In that case, the "ratio"
       will simply equal the other pointer. If both are zero, the "ratio"
       data will always be zero.
  */
  void updateData(); // same as setData, but uses old sources
  static void setFlip(bool donorflipx, bool donorflipy,
	       bool acceptorflipx, bool acceptorflipy);
  /*:F setFlip
   *:D Specifies whether the data is flipped in the x- and or y-dimensions
       relative to the ROI.
   *:N This is a static function that applies to any ROI3Datas created in the
       future. It DOES NOT affect pre-existing ROI3Datas.
  */    
  void setDebleach(DEBLEACH d);
  /*:F setDebleach
   *:D Specifies how we debleach.
       This does not cause recomputation and is thus trivially fast.
   */
  int getRatioNFrames() const;
  double getRatioT0ms() const;
  double getRatioDTms() const;
  double getRatioDurms() const;
  /*:F getRatioNFrames, getRatioT0ms, getRatioDTms, getRatioDurms
   *:D Timing information of the ratio trace.
   *:D If there are two cameras, then the ratio trace is the ratio of donor
       over acceptor, or actually the dF/F of the donor minusthe dF/F of the
       acceptor. If there is only one camera, then the ratio trace is just the
       dF/F of that camera.
   *:D t0 is the start time of the first frame; dt is the frame period;
       dur is the duration of each frame
   */
  int getAcceptorNFrames() const;
  double getAcceptorT0ms() const;
  double getAcceptorDTms() const;
  double getAcceptorDurms() const;
  int getDonorNFrames() const;
  double getDonorT0ms() const;
  double getDonorDTms() const;
  double getDonorDurms() const;
  Range timeRange() const;
  double const *dataDonor() const;
  /*:F dataDonor
   *:D Returns the debleached and normalized data dF/F of the donor
       in percents.
       This recomputes if necessary but returns cached results whenever
       possible.
   *:N The returned pointer is valid until the next call to getRaw or
       until this ROIData gets destructed.
  */       
  double const *dataAcceptor() const;
  /*:F dataAcceptor
   *:D Like dataDonor, but for the acceptor image sequence.
   */
  double const *dataRatio() const;
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
  void report() const;
protected:
  ROIData datDonor, datAcceptor;
  mutable QVector<double> datRatio;
  DEBLEACH debleach;
  double t0Ratio_ms, dtRatio_ms, durRatio_ms;
  mutable bool validRatio;
  bool haveRatio;
};

#endif
