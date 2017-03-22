// base/blobroi.h - This file is part of VScope.
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

// blobroi.h

#ifndef BLOBROI_H

#define BLOBROI_H

#include <base/types.h>
#include <base/transform.h>
#include <QVector>

class BlobROI {
  /*:C BlobROI
   *:D A BlobROI is a polygonal region of interest.
       BlobROIs can be thought of as a form of a PolyBlob that can be used
       to rapidly extract pixels from a ccd image.
   */
public:
  BlobROI(class PolyBlob const &src, Transform const &t, double border=0);
  BlobROI(class PolyBlob const &src, Transform const &t,
	  QRect bbox, double border=0);
  /*:F constructor
   *:D Creates a BlobROI from a PolyBlob, optionally blurring the edge by
       BORDER pixels.
   *:N This is not very fast: the current implementation has one call to
       PolyBlob's weight() for every pixel in the smallest rectangle
       entirely containing the PolyBlob.
  */
  ~BlobROI();
  class Result {
  public:
    Result();
    double mean() const; // mean pixel value within the ROI
    double std() const; // std dev. of pixel values within the ROI
    double npix() const; // number of pixels in the ROI (not affected by blur)
    double min() const; // min. pixel value in the ROI (not affected by blur)
    double max() const; // max. pixel value in the ROI (not affected by blur)
  private:
    double mean_;
    double std_;
    double npix_;
    double min_;
    double max_;
    friend class BlobROI;
  };
  int nPixels() const;
  /*:F nPixels
   *:D Returns number of pixels to allocate for the destination buffer
       for extract().
  */
  int bitmapX0() const { return x0; }
  int bitmapY0() const { return y0; }
  int bitmapW() const { return w; }
  int bitmapH() const { return h; }
  /*:F bitmapX0, bitmapY0, bitmapW, bitmapH
   *:D Return position and dimensions of bitmap and weight map.
   */
  int bitmapNPixels() const { return w*h; }
  /*:F bitmapNPixels
   *:D Returns number of booleans to allocate for the destination buffer
       for bitmap().
  */
  double const *weightmap() const { return weight.constData(); }
  /*:F weightmap
   *:D Returns a pointer to a WxH array of doubles with weights for each
       pixel. Weights are 1 inside, 0 outside, and intermediate near the
       edge, depending on the border width.
  */
  QVector<bool> bitmap() const;
  int bitmap(bool *dst, int dstSize) const;
  /*:F bitmap
   *:D Returns a binarized version of the weight map. You must allocate enough
       space before calling; use bitmapNPixels to find out how much.
   *:R Number of bools written or 0 if dstSize was not big enough.
   *:N The QVector version automatically resizes as needed
  */
private:
  int x0; // x_left of defined area
  int w;  // width of defined area
  int y0; // y_top of defined area
  int h; // height of defined area
  QVector<double> weight; // width x height area of weights for pixels
  /* Weights are 1 inside, 0 outside, and intermediate near border,
     dep. on blur.
  */
  double sumw;
  int npix;
private:
  void construct(PolyBlob const &src, Transform const &t,
		 QRect bbox, double border);
private: // not defined
  BlobROI(BlobROI const &other);
  BlobROI &operator=(BlobROI const &other);
};

#endif
