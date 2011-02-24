// roiimage.h

#ifndef ROIIMAGE_H

#define ROIIMAGE_H

#include <gfx/ccdimage.h>
#include <QDomElement>
#include <base/roicoords.h>

class ROIImage: public CCDImage {
  /*:C ROIImage
   *:D Extension of ROIImage that supports display of ROIs, both
       xyrra and blob styles.
   */
  Q_OBJECT;
public:
  enum ShowMode { // how to display with non-selected ROIs
    SM_Hidden=0,
    SM_Centers,
    SM_Outlines,
    SM_IDs,
    SM_Full,
  };
  enum ClickMode {
    CM_None=0,
    CM_Zoom,
    CM_SelectROI=100,
    CM_AddROI,
    CM_DelROI,
    CM_MoveROI,
    CM_ResizeROI,
    CM_RotateROI,
    CM_RotSizeROI,
    CM_BlobROI,
  };
public:
  ROIImage(QWidget *parent);
  virtual ~ROIImage();
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the current image and any ROIs
   */
  class ROISet *getROIs() const;
  /*:F getROIs
   *:D Returns a reference to the current set of ROIs.
   */
  void setROIs(class ROISet *);
  /*:F setROIs
   *:D Replaces the current set of ROIs.
   *:N We don't make a copy and may be modifying the set upon user
       interaction.
   */
public slots:
  void setMode(ClickMode cm);
  void showROIs(ShowMode sm);
  /*:F showROIs
   *:D Enables plotting of ROIs on top of the image
   */
  void updateSelection(int id);
  /*:F updateSelection
   *:D Slot to receive newSelection() signals from a partner image.
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
   *:D Zooms in 2x from the current view, recentering on selected ROI if any.
  */       
  virtual void updateZoom(QRect);
  /*:F updateZoom
   *:D Updates this image's zoom settings without emitting a shareZoom signal.
       This is typically the recipient of a shareZoom from another image.
  */
 signals:
  void newSelection(int id);
  /*:S newSelection
   *:D Emitted whenever a new ROI is selected.
   */
private:
  void showSelectedID(bool show=true);
  void hideSelectedID();
protected:
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
  virtual void resizeEvent(class QResizeEvent *);
  int findNearestROI(QPoint xy, double marg=0); // xy, marg in screen coords!
  void selectNearestROI(QPoint xy, double marg=0);
  /*:F selectNearestROI
   *:D Selects the ROI nearest xy. If none, do nothing.
   *:N xy and marg are in screen coords.
   */
private:
  void recalcEllipse();
  /*:F recalcEllipse
   *:D Sets the shape of the ellipse to match the currently selected
       ROI properly adjusted for our zoom level.
  */
  void select(int id);
public:
  int currentROI() const { return selectedroi; }
protected:
  int selectedroi; // ID of selected ROI or 0 if none.
  ClickMode clickMode; // what to do when user clicks mouse
  ShowMode showMode; // how non-selected ROIs are shown
  class Ellipse *ellipse; // used for current XYRRA ROI
  class VisiBlob *visiblob; // used for current PolyBlob ROI
  class ROISet *roiset;
  static class ROISet *dummyrois;
  ROICoords *editing;
};

#endif
