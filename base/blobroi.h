// blobroi.h

#ifndef BLOBROI_H

#define BLOBROI_H

#include <base/types.h>
#include <base/transform.h>

class BlobROI_ {
protected:
  int x0; // x_left of defined area
  int w;  // width of defined area
  int y0; // y_top of defined area
  int h; // height of defined area
  double *weight; // width x height area of weights for pixels
  /* Weights are 1 inside, 0 outside, and intermediate near border,
     dep. on blur.
  */
  double sumw;
  int npix;
};

class BlobROI: private BlobROI_ {
  /*:C BlobROI
   *:D A BlobROI is a polygonal region of interest.
       BlobROIs can be thought of as a form of a PolyBlob that can be used
       to rapidly extract pixels from a ccd image.
   */
public:
  BlobROI(class PolyBlob const &src, Transform const &t, double border=0);
  /*:F constructor
   *:D Creates a BlobROI from a PolyBlob, optionally blurring the edge by
       BORDER pixels.
   *:N This is not very fast: the current implementation has one call to
       PolyBlob's weight() for every pixel in the smallest rectangle
       entirely containing the PolyBlob.
  */
  ~BlobROI();
  BlobROI(BlobROI const &other);
  BlobROI &operator=(BlobROI const &other);
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
  double *weightmap() const { return weight; }
  /*:F weightmap
   *:D Returns a pointer to a WxH array of doubles with weights for each
       pixel. Weights are 1 inside, 0 outside, and intermediate near the
       edge, depending on the border width.
  */
  int bitmap(bool *dst, int dstSize) const;
  /*:F bitmap
   *:D Returns a binarized version of the weight map. You must allocate enough
       space before calling; use bitmapNPixels to find out how much.
   *:R Number of bools written or 0 if dstSize was not big enough.
  */
private:
  void copy(BlobROI const &other);
};

#endif
