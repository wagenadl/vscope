// gfx/ccdimage.h - This file is part of VScope.
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

// ccdimage.h

#ifndef CCDImage_H
#define CCDImage_H

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QString>
#include <base/types.h>
#include <base/xyrra.h>
#include <base/transform.h>
#include <math/floatimage.h>

class CCDImage_ {
public: // everything is initialized in CCDImage's constructor
  QImage image; // currently displayed image
  uint16_t min, max; // color map ranges
  QRect zoomRect; // current zoom, in canvas coords
  QRect canvasRect; // null means: same as widget
  Transform img2cnv; // does not reflect zoom
  Transform cnv2scr; // reflects current zoom
  class QRubberBand *rubberband; // used during zoom dragging
  QPoint clickPoint; // location of most recent mouse press
  double adjust_black, adjust_white;
  QVector<float> gamma_table;
  FloatImage origImage;
  FloatImage gammaImage;
  FloatImage aceImage; //  only defined if acePercentage>0
  double aceFraction;
  bool hasACE;
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
  virtual void newImage(uint16_t const *data, int X, int Y, Transform const &t);
  virtual void newImage(uint16_t const *data, int X, int Y,
			bool flipX, bool flipY);
  /*:F newImage
   *:D Provides new data to be used for drawing images.
   *:N The data is copied into a QImage, and does not need to remain
       available.
   *:N You must call autoRange or setRange before first calling newImage,
       because there is no default range.
   *:D Places the (unzoomed) image at a certain position in the widget's
       world coordinates.
   *:N For instance, to place a 256x128 image in the center 256x256 area
       of a 512x512 canvas, you would say:
         placeImage(Transform().scale(1,2).translate(128.128)).
  */
  void black();
  virtual void setCanvas(QRect canvasExtent);
  virtual void resetCanvas();
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
   *:D Zooms in to the given rectangle within the canvas.
   *:N Rectangle is in canvas coordinates.
  */
  virtual void resetZoom();
  /*:F resetZoom
   *:D Zooms out to show the entire canvas
   */
  virtual void zoomIn();
  /*:F zoomIn
   *:D Zooms in 2x from the current view.
  */       
  virtual void zoomIn(int x0, int y0);
  virtual void zoomIn(QPoint xy0);
  /*:F zoomIn
   *:D Zooms in 2x from the current view, recentered on (x0,y0).
   *:N x0,y0 in canvas coordinates
  */       
  virtual void zoomOut();
  /*:F zoomOut
   *:D Zooms out 2x from the current view or back to viewing the whole canvas.
   */
  virtual void updateZoom(QRect);
  /*:F updateZoom
   *:D Updates this image's zoom settings without emitting a shareZoom signal.
       This is typically the recipient of a shareZoom from another image.
   *:N Rectangle must be in canvas coordinates
  */
signals:
  void newZoom(QRect);
  /*:S newZoom
   *:D Emitted whenever our zoom is explicitly changed. See sharedZoom.
   *:N Rectangle is in canvas coordinates.
   */
protected:
  void constrainZoom();
  /*:F constrainZoom
   *:D Makes zoom's aspect ratio match canvas aspect ratio and fit
       inside canvas.
   *:N This does *not* emit shareZoom.
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
  void rebuildGammaTable();
  void rebuildImage();
public:
  Transform const &imageToCanvas() { return img2cnv; }
  Transform const &canvasToScreen() { return cnv2scr; }
  QImage const &currentImage() const { return image; }
  void overwriteImage(QImage const &img);
  QRect const &currentZoom() const { return zoomRect; }
  QRect currentCanvas() const;
};

#endif
