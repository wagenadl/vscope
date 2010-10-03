// ccdimage.h

#ifndef CCDImage_H
#define CCDImage_H

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QString>
#include <base/types.h>
#include <base/xyrra.h>

class CCDImage_ {
  protected:
  QImage image; // currently displayed image
  uint16_t min, max; // color map ranges
  QRect zoomRect; // current zoom, or undefined if not zoomed in
  bool hasZoom; // are we at all zoomed in?
  class QRubberBand *rubberband; // used during zoom dragging
  QPoint clickPoint; // location of most recent mouse press
  double adjust_black, adjust_white;
  int *gamma_table;
};

class CCDImage: public QWidget, protected CCDImage_ {
  /*:C CCDImage
   *:D shows an image from a CCD camera.
   *:N Supports zooming and flipping.
   */
  Q_OBJECT;
public:
  CCDImage(QWidget *parent);
  /*:F constructor
   *:D creates a new widget with a default (test) image.
   */
  virtual ~CCDImage();
private: // these are not implemented:
  CCDImage(CCDImage const &other);
  CCDImage &operator=(CCDImage const &other);
public:
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the current image
   */
public slots:
  virtual void newImage(uint16_t const *data, int X, int Y,
			bool flipX=false, bool flipY=false);
  /*:F newImage
   *:D Provides new data to be used for drawing images.
   *:N The data is copied into a QImage, and does not need to remain
       available.
   *:N You must call autoRange or setRange before first calling newImage,
       because there is no default range.
  */
  virtual void recolor(QString);
  /*:F recolor
   *:D Changes black or white level. The string argument must be one of:
       - blackMin
       - blackPlus
       - whiteMin
       - whitePlus
       - reset
  */
  void adjustedRange(uint16_t const *data, int X, int Y);
  /*:F adjustedRange
   *:D Sets the color table for subsequent images based on some image data.
   *:N How much range is sub-black or super-white is defined by the
       internal variables adjust_black and adjust_white, which can be
       changed through recolor.
  */
  void autoRange(uint16_t const *data, int X, int Y,
		 double frc0=0, double frc1=1);
  /*:F autoRange
   *:D Sets the color table for subsequent images based on some image data.
   *:A If frc0>0, black corresponds not to the absolute minimum found, but
       to the frc0-th fractile.
       If frc1<1, white corresponds not to the absolute minimum found, but
       to the frc1-th fractile.
   *:N Note that setting a range only affects new images; the current display
       remains unchanged.
  */
  void setRange(uint16_t min, uint16_t max);
  /*:F setRange
   *:D Sets the color table for subsequent images based on specific numbers.
   *:N Note that setting a range only affects new images; the current display
       remains unchanged.
  */
  virtual void setZoom(QRect const &zoom);
  /*:F setZoom
   *:D Zooms in to the given rectangle within the image. Note that if this
       rectangle has a different aspect ratio than the image, ROIs may be
       plotted incorrectly.
  */
  virtual void resetZoom();
  /*:F resetZoom
   *:D Zooms out to show the entire image
   */
  virtual void zoomIn();
  /*:F zoomIn
   *:D Zooms in 2x from the current view.
  */       
  virtual void zoomIn(int x0, int y0);
  /*:F zoomIn
   *:D Zooms in 2x from the current view, recentered on (x0,y0).
  */       
  virtual void zoomOut();
  /*:F zoomOut
   *:D Zooms out 2x from the current view or back to viewing the whole image.
   */
  virtual void sharedZoom(bool,QRect);
  /*:F sharedZoom
   *:D Updates this image's zoom settings without emitting a shareZoom signal.
       This is typically the recipient of a shareZoom from another image.
  */
signals:
  void shareZoom(bool, QRect);
  /*:S shareZoom
   *:D Emitted whenever our zoom is explicitly changed. See sharedZoom.
   */
protected:
  void createTestImage();
  /*:F createTestImage
   *:D Produces a colorful test image.
   */
  void constrainZoom();
  /*:F constrainZoom
   *:D Makes zoom square and within image. This does *not* emit shareZoom.
   */
  virtual void mousePressEvent(class QMouseEvent *);
  /*:F mousePressEvent
   *:D Response to mouse click depends on current click mode.
       If it is ZOOM, this initiates dragging a zoom rectangle.
  */
  virtual void mouseReleaseEvent(class QMouseEvent *);
  /*:F mouseReleaseEvent
   *:D Response to mouse release depends on current click mode.
       If it is ZOOM, this ends dragging a zoom rectangle. If the rectangle
         is larger than nothing, we zoom in to the smallest containing
	 square; otherwise, we simply zoom in 2x on the clicked point.
   *:N We take care to ignore release events that are associated with
       double clicks.
  */
  virtual void mouseMoveEvent(class QMouseEvent *);
  /*:F mouseMoveEvent
   *:D Internal house keeping during drags.
   */
  virtual void mouseDoubleClickEvent(class QMouseEvent *);
  /*:F mouseDoubleClickEvent
   *:D Response to double clicks depends on current click mode.
       If it is ZOOM, we zoom out to show the entire image.
  */
protected:
  XYRRA screenToImage(XYRRA el) const;
  XYRRA imageToScreen(XYRRA el) const;
  QPointF screenToImage(QPoint xy) const;
  QPoint imageToScreen(QPointF xy) const;
  QRect screenToImage(QRect r) const;
  QRect imageToScreen(QRect r) const;
  double screenToImage(double length) const; // approx!
  double imageToScreen(double length) const; // approx!
  void rebuildGammaTable();
  QRect safeZoomRect() const;
public:
  QImage currentImage() const { return image; }
  void overwriteImage(QImage img);
protected:
  class ZoomInfo {
  public:
    ZoomInfo() { ax=1; ay=1; bx=0; by=0; }
    double ax, ay;
    double bx, by;
  };
  ZoomInfo makeZoomInfo();
};

#endif
