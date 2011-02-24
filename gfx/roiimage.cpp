// roiimage.cpp

#include "roiimage.h"
#include <base/exception.h>
#include <base/types.h>
#include <algorithm>
#include <QPainter>
#include <math.h>
#include <stdio.h>
#include <QMouseEvent>
#include <QRubberBand>
#include <base/dbg.h>
#include <gfx/ellipse.h>
#include <gfx/visiblob.h>
#include <base/roiset.h>
#include <base/xyrra.h>
#include <base/polyblob.h>
#include <base/numbers.h>
#include <base/base26.h>

ROISet *ROIImage::dummyrois = new ROISet;

inline double sq(double x) { return x*x; }

ROIImage::ROIImage(QWidget *parent): CCDImage(parent) {
  clickMode = CM_None;
  showMode = SM_Outlines;
  selectedroi = 0;
  editing = 0;
  roiset = dummyrois;
  ellipse = new Ellipse(this);
  /* The ellipse always uses screen coordinates rather than image coordinates
     when we are zoomed in.
  */
  ellipse->hide();
  ellipse->setPen(QColor("#ff0000"));

  visiblob = new VisiBlob(this);
  visiblob->hide();
  visiblob->setPen(QColor("#ff0000"));
}

ROIImage::~ROIImage() {
  if (ellipse)
    delete ellipse;
  if (visiblob)
    delete visiblob;
  if (editing)
    delete editing;
}  

ROISet *ROIImage::getROIs() const {
  return roiset;
}

void ROIImage::setROIs(ROISet *rs) {
  roiset = rs ? rs : dummyrois;
  select(0);
  update();
}

void ROIImage::setMode(ClickMode cm) {
  clickMode = cm;
}

void ROIImage::setZoom(QRect const &z) {
  dbg("roiimage: setzoom");
  CCDImage::setZoom(z);
  recalcEllipse();
  update();
}

void ROIImage::resetZoom() {
  CCDImage::resetZoom();
  recalcEllipse();
  update();
}

void ROIImage::zoomIn() {
  dbg("roiimage: zoomin");
  if (selectedroi>0) 
    if (!roiset->contains(selectedroi)) 
      select(0);
  
  if (selectedroi>0) 
    CCDImage::zoomIn(int(roiset->centerX(selectedroi)),
		     int(roiset->centerY(selectedroi)));
  else 
    CCDImage::zoomIn();
}

void ROIImage::updateZoom(QRect rect) {
  CCDImage::updateZoom(rect);
  recalcEllipse();
  update();
}

void ROIImage::paintEvent(class QPaintEvent *e) {
  //  dbg("roiimage:paintevent");
  CCDImage::paintEvent(e);
  QPainter p(this);
  p.setPen(QColor("#ffff00"));
  if (showMode==SM_Centers)
    p.setBrush(QColor("#ffff00"));
  QPen basepen = p.pen();

  ZoomInfo z = makeZoomInfo();
  
  foreach (int id, roiset->ids()) {
    bool showID = showMode==SM_Full || showMode==SM_IDs;
    bool showDot = showMode==SM_Centers && !id==selectedroi;
    bool showOutline = showMode==SM_Full || showMode==SM_Outlines ||
      id==selectedroi;
    if (editing && id==selectedroi)
      showID = showDot = showOutline = false;

    double x0 = z.ax * roiset->centerX(id) + z.bx;
    double y0 = z.ay * roiset->centerY(id) + z.by;
    if (showDot) 
      p.drawEllipse(QPointF(x0,y0),2,2);

    if (showOutline) {
      ROICoords roi=roiset->get(id);
      if (roi.isXYRRA()) 
	imageToScreen(roi.xyrra()).paint(&p);
      else if (roi.isBlob()) 
	roi.blob().paint(&p, z.ax,z.bx, z.ay,z.by);
    }
    if (showID) {
      if (id==selectedroi)
	p.setPen(QColor("#ff0000"));
      p.drawText(QRectF(x0-1,y0-1,2,2),
		 Qt::AlignCenter | Qt::TextDontClip,
		 num2az(id));
      if (id==selectedroi)
	p.setPen(basepen);
    }
  }
  //  dbg("roiimage:paintevent: done with painter");
}

void ROIImage::mousePressEvent(QMouseEvent *e) {
  dbg("ROIImage: press (%i,%i) mode=%i\n",e->x(),e->y(),clickMode);
  clickPoint = e->pos();
  switch (clickMode) {
  case CM_Zoom: case CM_None:
    CCDImage::mousePressEvent(e);
    break;
  case CM_SelectROI:
    // perhaps this should be done on release instead?
    select(findNearestROI(clickPoint,5));
    break;
  case CM_AddROI: { // This adds a XYRRA
    select(0);
    editing = new ROICoords();
    editing->makeXYRRA();
    editing->xyrra() = XYRRA(clickPoint.x(),clickPoint.y());
    recalcEllipse();
    ellipse->startCreate(e);
  } break;
  case CM_MoveROI: {
    selectNearestROI(clickPoint,5);
    if (selectedroi) {
      editing = new ROICoords(roiset->get(selectedroi));
      recalcEllipse();
      if (editing->isXYRRA())
	ellipse->startMove(e);
      else if (editing->isBlob())
	visiblob->startMove(e);
    }
  } break;
  case CM_ResizeROI: 
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->isXYRRA(selectedroi)) {
      editing = new ROICoords(roiset->get(selectedroi));
      ellipse->startResize(e);
    }
    break;
  case CM_RotateROI: 
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->isXYRRA(selectedroi)) {
      ellipse->startRotate(e);
    }
    break;
  case CM_RotSizeROI: 
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->isXYRRA(selectedroi)) {
      editing = new ROICoords(roiset->get(selectedroi));
      ellipse->startRotSize(e);
    }
    break;
  case CM_DelROI: {
    selectNearestROI(clickPoint,0);
    roiset->remove(selectedroi);
  } break;
  case CM_BlobROI: {
    /* From the Matlab version:
         Semantics are:
	 If nothing was selected, this may select a blob, or, if none near,
	   start creating a new one.
         If there was a selection, this moves the edge if the click was closer 
	   to the edge then to the center, and if it was no more than 2 pixels
	   outside of the greatest diameter.
    */
    if (selectedroi) {
      if (roiset->isPoly(selectedroi)) {
	// We have a polyblob selection, so either we'll modify the blob,
	// or we'll deselect.
	PolyBlob const &roip = roiset->get(selectedroi).blob();
	ZoomInfo z = makeZoomInfo();
	double x = (e->x()-z.bx)/z.ax;
	double y = (e->y()-z.by)/z.ay;
	double dr0 = roip.distToCenter(x,y);
	double dr1 = roip.distToEdge(x,y);
	if (dr0<dr1) {
	  // closer to center than to edge -> deselect
	  dbg("roiimage:blobroi: click by center -> deselect");
  	  select(0);
	  break; // we won't reselect!
	} else if (dr0 < roip.greatestRadius() + 2) {
	  // we're inside the max radius, so let's distort
	  dbg("roiimage:blobroi: click by edge -> distort");
	  editing = new ROICoords(roip);
	  visiblob->startAdjust(e);
	} else {
	  // we're too far away, so let's deselect, and pos'bly reselect/create
	  dbg("roiimage:blobroi: click far -> deselect but try s/th else");
	  select(0); // triggers following if
	}
      } else {
	// We have a non-polyblob selection, so let's just deselect
	// Not sure if this is the most obvious response
	dbg("roiimage:blobroi: not a polyblob -> deselect");
	select(0);
	break;
      }
    }
    if (!selectedroi) {
      // no selection
      dbg("roiimage:blobroi: click w/o prior selection");
      int id = findNearestROI(clickPoint,0);
      select(id);
      if (id) {
	dbg("  -> selected %i",selectedroi);
      } else {
	// let's make a new one
	editing = new ROICoords();
	editing->makeBlob();
	ZoomInfo z = makeZoomInfo();
	visiblob->setZoom(z.ax,z.bx, z.ay,z.by); // make sure we have zoom right
	visiblob->show();
	visiblob->startCreate(&editing->blob(), e);
      }
    }
  } break;
  }
  update();
}

void ROIImage::selectNearestROI(QPoint xy, double margin) {
  int inroi = findNearestROI(xy, margin);
  if (inroi) 
    select(inroi);
}

int ROIImage::findNearestROI(QPoint xy, double marg) {
  QPointF xy_ = screenToImage(xy);
  int bestid=0;
  double dd;
  foreach (int id, roiset->ids()) {
    double d = sq(xy_.x()-roiset->centerX(id))
      +        sq(xy_.y()-roiset->centerY(id));
    if (bestid==0 || d<dd) {
      bestid = id;
      dd = d;
    }
  }
  if (bestid) 
    if (roiset->inside(bestid, xy_.x(), xy_.y(), screenToImage(marg)))
      return bestid;
  return 0;
}

void ROIImage::mouseReleaseEvent(QMouseEvent *e) {
  dbg("ROIImage: release (%i,%i) mode=%i\n",e->x(),e->y(),int(clickMode));
  if (clickMode==CM_Zoom || clickMode==CM_None) {
    CCDImage::mouseReleaseEvent(e);
  }
  if (editing) {
    bool toosmall = true;
    if (editing->isXYRRA()) {
      ellipse->complete(e);
      *editing = screenToImage(ellipse->getShape());
      toosmall = editing->xyrra().R < 3 || editing->xyrra().r < 3;
    } else if (editing->isBlob()) {
      visiblob->complete(e);
      toosmall = editing->blob().greatestRadius() < 3;
    }
    if (toosmall) {
      if (selectedroi)
	roiset->remove(selectedroi);
    } else {
      int id = selectedroi ? selectedroi : roiset->newROI();
      roiset->checkout(id) = *editing;
      roiset->checkin(id);
      if (id!=selectedroi)
	select(id);
    }
    delete editing;
    editing = 0;
    recalcEllipse(); // i.e., remove it
    update();
  }
}

void ROIImage::updateSelection(int id) {
  selectedroi = roiset->contains(id) ? id : 0;
  update();
}

void ROIImage::mouseMoveEvent(QMouseEvent *e) {
  if (clickMode==CM_Zoom || clickMode==CM_None) {
    CCDImage::mouseMoveEvent(e);
  }
  if (editing) {
    if (editing->isXYRRA())
      ellipse->drag(e);
    else if (editing->isBlob())
      visiblob->drag(e);
  }
}

void ROIImage::mouseDoubleClickEvent(QMouseEvent *e) {
  //printf("ROIImage: double Click (%i,%i)\n",e->x(),e->y());
  if (editing) {
    delete editing;
    editing = 0;
    update();
  }
  switch (clickMode) {
  case CM_Zoom: case CM_None:
    CCDImage::mouseDoubleClickEvent(e);
    break;
  default:
    break;
  }
}

void ROIImage::resizeEvent(class QResizeEvent *) {
  if (ellipse)
    ellipse->setGeometry(rect());
  if (visiblob)
    visiblob->setGeometry(rect());
}

void ROIImage::recalcEllipse() {
  if (editing && editing->isXYRRA()) {
    ellipse->setShape(imageToScreen(editing->xyrra()));
    ellipse->show();
  } else {
    ellipse->hide();
  }
  if (editing && editing->isBlob()) {
    visiblob->setShape(&editing->blob(), false);
    ZoomInfo z = makeZoomInfo();
    visiblob->setZoom(z.ax,z.bx, z.ay,z.by);
    visiblob->show();
  } else {
    visiblob->hide();
  }
}

void ROIImage::showROIs(ShowMode sm) {
  showMode = sm;
  update();
}

void ROIImage::select(int id) {
  updateSelection(id);
  emit newSelection(id);
}
