
// roidata.h

#ifndef ROIDATA_H

#define ROIDATA_H

#include <base/roicoords.h>

class ROIData_ {
public:
  enum Debleach { // This *must* match DEBLEACH in parameters.xml
    None=0, Linear=1, Quadratic=2, Exponential=3,
  };
private: friend class ROIData;
  double *dataRaw;
  double *dataDebleached;
  int lengthRaw;
  int lengthDebleached;
  Debleach debleach;
  bool validRaw;
  bool validDebleached;
  class CCDData const *source;
  ROICoords const *roi;
  class BlobROI *blobROI;
  bool validBlobROI;
  bool *bitmap;
  bool validBitmap;
  int xl, yt, w, h;
  int npix; // number of pixels inside the ROI
  bool flipX, flipY;
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
  ROIData(ROIData const &other);
  ROIData &operator=(ROIData const &other);
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
  void setData(class CCDData const *source);
  /*:F setData
   *:D Specifies which data we are talking about. This does not immediately
       cause recalculation, rather, recalculation is postponed until the
       next call to getRaw() or getDebleached().
       While this is a very efficient way of doing business, it does imply
       that the data must remain available until the next call to setData.
  */
  void setFlip(bool flipx, bool flipy);
  /*:F setFlip
   *:D Specifies whether the data is flipped in the x- and or y-dimensions
       relative to the ROI.
  */
  double const *getRaw();
  /*:F getRaw
   *:D Returns the time dependent average fluorescence for our ROI in
       the current data. This recomputes if necessary but returns cached
       results whenever possible.
   *:N The returned pointer is valid until the next call to getRaw or
       until this ROIData gets destructed.
  */
  double const *getDebleachedDFF();
  /*:F getDebleachedDFF
   *:D Like getRaw(), but applies debleaching as set by setDebleach, and
       normalizes the result.
   *:R Normalized debleached data in percent.
   */
  int getNFrames() const;
  bool haveData() const;
private:
  void ensureBitmap();
  void makePolyBitmap();
  void makeXYRRABitmap();
  void makeNullBitmap();
private:
  void copy(ROIData const &other);
};

#endif
