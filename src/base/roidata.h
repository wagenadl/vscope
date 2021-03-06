// base/roidata.h - This file is part of VScope.
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


// roidata.h

#ifndef ROIDATA_H

#define ROIDATA_H

#include <base/roicoords.h>
#include <base/range.h>
#include <base/transform.h>
#include <base/cache.h>
#include <base/enums.h>

class ROIData {
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
  void setDebleach(DEBLEACH s);
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
  void updateData();
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
  double getDurms() const;
  Range timeRange() const;
  bool haveData() const;
  void report() const;
private:
  bool ensureBitmap() const; // returns true iff it worked
private:
  class BitmapCache: public Cache {
  public:
    BitmapCache();
    ~BitmapCache();
    bool const *bitmap() const;
    QRect const &rect() const;
    int npixels() const;
    void setROI(ROICoords const *roi);
    void setTransformAndClip(Transform const &t, QRect const &clip);
    void unsetTransformAndClip();
  protected:
    virtual void validator();
  private:
    QVector<bool> bm;
    QRect rec; // in image coordinates
    int npix; // number of pixels inside the ROI
    class BlobROI *blobROI;
    ROICoords const *roi; // we don't own this
    Transform t;
    QRect clip; // of image
    bool haveTransformAndClip;
  private:
    void makePolyBitmap(PolyBlob const &roi);
    void makeXYRRABitmap(XYRRA const &roi);
  };
  class DataCache: public Cache {
  public:
    DataCache();
    ~DataCache();
    double *data() { return dat.data(); }
    int length() const { return dat.size(); }
    void resize(int len);
  private:
    QVector<double> dat;
  };
  class CCDData const *source;
  DEBLEACH debleach;

  mutable DataCache raw;
  mutable DataCache debleached;
  mutable BitmapCache bitmap;
private: // not defined:
  ROIData(ROIData const &);
  ROIData &operator=(ROIData const &);
};

#endif
