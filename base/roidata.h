
// roidata.h

#ifndef ROIDATA_H

#define ROIDATA_H

#include <base/roicoords.h>
#include <base/range.h>
#include <base/transform.h>

class ROIData_ {
public:
  enum Debleach { // This *must* match DEBLEACH in parameters.xml
    None=0, Linear=1, Quadratic=2, Exponential=3,
  };
private: friend class ROIData;
  mutable double *dataRaw;
  mutable double *dataDebleached;
  mutable int lengthRaw;
  mutable int lengthDebleached;
  Debleach debleach;
  mutable bool validRaw;
  mutable bool validDebleached;
  class CCDData const *source;
  ROICoords const *roi; // in global coordinates
  mutable class BlobROI *blobROI;
  mutable bool validBlobROI;
  mutable bool *bitmap;
  mutable bool validBitmap;
  mutable int xl, yt, w, h; // in image coordinates
  mutable int npix; // number of pixels inside the ROI
  bool flipX, flipY;
  Transform tinv;
  bool validTransform;
};

class ROIData: public ROIData_ {
  /*:C ROIData
   *:D Data from a specific image for a given ROI.
   *:N This class maintains a bitmap for the ROI so that average data
       can be quickly extracted from that same ROI in multiple images.
   *:N This class can handle both xyrra-style (elliptic) and polyblob-style
       (polygonal) ROIs.
  */
public:
  ROIData();
  ~ROIData();
  void setDebleach(Debleach d);
  /*:F setDebleach
   *:D Specifies how DebleachedDFF gets calculated.
   *:N This does not cause immediate recomputation.
   */
  void setROI(ROICoords const *roi);
  /*:F setROI
   *:D Redefines which portion of the CCD image we care about.
       This does not cause recomputation and is thus trivially fast.
   *:N The ROI is not copied, and must remain available until the next
       call to setROI.
   *:N If ROI is null, all results will be zero.
  */
  void setData(class CCDData const *source, bool noemit=false);
  /*:F setData
   *:D Specifies which data we are talking about. This does not immediately
       cause recalculation, rather, recalculation is postponed until the
       next call to getRaw() or getDebleached().
       While this is a very efficient way of doing business, it does imply
       that the data must remain available until the next call to setData.
  */
  void updateData(bool noemit=false);
  void setFlip(bool flipx, bool flipy);
  /*:F setFlip
   *:D Specifies whether the data is flipped in the x- and or y-dimensions
       relative to the ROI.
  */
  double const *getRaw() const;
  /*:F getRaw
   *:D Returns the time dependent average fluorescence for our ROI in
       the current data. This recomputes if necessary but returns cached
       results whenever possible.
   *:N The returned pointer is valid until the next call to getRaw or
       until this ROIData gets destructed.
  */
  double const *getDebleachedDFF() const;
  /*:F getDebleachedDFF
   *:D Like getRaw(), but applies debleaching as set by setDebleach, and
       normalizes the result.
   *:R Normalized debleached data in percent.
   */
  int getNFrames() const;
  double getT0ms() const;
  double getDTms() const;
  Range timeRange() const;
  bool haveData() const;
private:
  bool ensureBitmap() const; // returns true iff it worked
  bool makePolyBitmap() const;
  bool makeXYRRABitmap() const;
  bool makeNullBitmap() const;
};

#endif
