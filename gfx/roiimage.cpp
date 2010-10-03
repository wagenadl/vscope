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
  selectedIDShown = true;
  selectedroi = 0;
  justcreatedroi = 0;
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
}  

ROISet *ROIImage::getROIs() const {
  return roiset;
}

void ROIImage::setROIs(ROISet *rs) {
  roiset = rs;
  acceptROIselect(0);
  emit selectedROI(0);
  update();
}

int ROIImage::ensureIdNew(int id) {
  while (roiset->contains(id)) 
    id++;
  return id;
}

void ROIImage::setModeBlobROI() {
  clickMode = CM_BlobROI;
}

void ROIImage::setModeSelectROI() {
  clickMode = CM_SelectROI;
}

void ROIImage::setModeAddROI() {
  clickMode = CM_AddROI;
}

void ROIImage::setModeDelROI() {
  clickMode = CM_DelROI;
}

void ROIImage::setModeMoveROI() {
  clickMode = CM_MoveROI;
}

void ROIImage::setModeResizeROI() {
  clickMode = CM_ResizeROI;
}

void ROIImage::setModeRotateROI() {
  clickMode = CM_RotateROI;
}

void ROIImage::setModeRotSizeROI() {
  clickMode = CM_RotSizeROI;
}

void ROIImage::setModeZoom() {
  clickMode = CM_Zoom;
}

void ROIImage::deleteAllROIs() {
  roiset->clear();
  acceptROIselect(0);
  emit selectedROI(0);
  update();
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
      acceptROIselect(0);
  
  if (selectedroi>0) {
    // zoom in to selected ROI
    CCDImage::zoomIn(int(roiset->centerX(selectedroi)),
		     int(roiset->centerY(selectedroi)));
  } else {
    CCDImage::zoomIn();
  }
}

void ROIImage::sharedZoom(bool has, QRect rect) {
  dbg("roiimage: sharedzoom");
  CCDImage::sharedZoom(has, rect);
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

  ZoomInfo z = makeZoomInfo();
  
  QSet<int> const &ids = roiset->ids();
  for (QSet<int>::const_iterator i=ids.begin(); i!=ids.end(); i++) {
    int id = *i;
    if (id==selectedroi) {
      ; // the ellipse or visiblob takes care of us, except for label
      if ((showMode==SM_Full || showMode==SM_IDs) && selectedIDShown) {
	double x0 = z.ax * roiset->centerX(id) + z.bx;
	double y0 = z.ay * roiset->centerY(id) + z.by;
	QPen x = p.pen();
	p.setPen(QColor("#ff0000"));
	p.drawText(QRectF(x0-1,y0-1,2,2),
		   Qt::AlignCenter | Qt::TextDontClip,
		   num2az(id));
	p.setPen(x);
      }
    } else {
      double x0 = z.ax * roiset->centerX(id) + z.bx;
      double y0 = z.ay * roiset->centerY(id) + z.by;
      switch (showMode) {
      case SM_Hidden:
	break;
      case SM_Centers:
	p.drawEllipse(QPointF(x0,y0),2,2);
	break;
      case SM_Outlines: case SM_Full:
	if (roiset->isXYRRA(id)) {
	  XYRRA el = imageToScreen(roiset->get(id));
	  el.paint(&p);
	} else if (roiset->isPoly(id)) {
	  PolyBlob const &pb = roiset->getp(id);
	  pb.paint(&p, z.ax,z.bx, z.ay,z.by);
	} else {
	  // unknown ROI style
	}
	if (showMode==SM_Outlines)
	  break;
	// Fall through for SM_Full
      case SM_IDs:
	p.drawText(QRectF(x0-1,y0-1,2,2),
		   Qt::AlignCenter | Qt::TextDontClip,
		   num2az(id));
	break;
      }
    }
  }
  //  dbg("roiimage:paintevent: done with painter");
}

void ROIImage::mousePressEvent(QMouseEvent *e) {
  justcreatedroi = 0;
  dbg("ROIImage: press (%i,%i) mode=%i\n",e->x(),e->y(),clickMode);
  clickPoint = e->pos();
  switch (clickMode) {
  case CM_Zoom: case CM_None:
    CCDImage::mousePressEvent(e);
    break;
  case CM_SelectROI:
    // perhaps this should be done on release?
    acceptROIselect(findNearestROI(clickPoint,5));
    emit selectedROI(selectedroi);
    break;
  case CM_AddROI: { // This adds a XYRRA
    selectedroi = justcreatedroi = ensureIdNew();
    XYRRA nwroi(clickPoint.x(),clickPoint.y());
    roiset->set(selectedroi, nwroi);
    recalcEllipse();
    ellipse->startCreate(e);
    hideSelectedID();
    dbg("CM_AddROI: nw %i = [(%g,%g)+(%g,%g)/%g]",selectedroi,
	nwroi.x0,nwroi.y0,nwroi.R,nwroi.r,nwroi.a);
  } break;
  case CM_MoveROI: // Only works on XYRRA
    selectNearestROI(clickPoint,5);
    if (selectedroi) {
      if (roiset->isXYRRA(selectedroi)) 
	ellipse->startMove(e);
      else if (roiset->isPoly(selectedroi))
	visiblob->startMove(e);
      else
	break; // hmmm...
      hideSelectedID();
    }
    break;
  case CM_ResizeROI: 
    // should we first select nearest?
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->isXYRRA(selectedroi)) {
      ellipse->startResize(e);
      hideSelectedID();
    }
    break;
  case CM_RotateROI: 
    // should we first select nearest?
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->isXYRRA(selectedroi)) {
      ellipse->startRotate(e);
      hideSelectedID();
    }
    break;
  case CM_RotSizeROI: 
    // should we first select nearest?
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->isXYRRA(selectedroi)) {
      ellipse->startRotSize(e);
      hideSelectedID();
    }
    break;
  case CM_DelROI: {
    selectedroi = findNearestROI(clickPoint,0);
    deleteROI();
    dbg("roiimage:delroi: delete roi %i from %p",selectedroi,roiset);
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
	PolyBlob const &roip = roiset->getp(selectedroi);
	ZoomInfo z = makeZoomInfo();
	double x = (e->x()-z.bx)/z.ax;
	double y = (e->y()-z.by)/z.ay;
	double dr0 = roip.distToCenter(x,y);
	double dr1 = roip.distToEdge(x,y);
	if (dr0<dr1) {
	  // closer to center than to edge -> deselect
	  dbg("roiimage:blobroi: click by center -> deselect");
  	  acceptROIselect(0);
	  emit selectedROI(0);
	  break; // we won't reselect!
	} else if (dr0 < roip.greatestRadius() + 2) {
	  // we're inside the max radius, so let's distort
	  dbg("roiimage:blobroi: click by edge -> distort");
	  visiblob->startAdjust(e);
	} else {
	  // we're too far away, so let's deselect, and pos'bly reselect/create
	  dbg("roiimage:blobroi: click far -> deselect but try s/th else");
	  selectedroi = 0; // triggers following if
	}
      } else {
	// We have a non-polyblob selection, so let's just deselect
	// Not sure if this is the most obvious response
	dbg("roiimage:blobroi: not a polyblob -> deselect");
	acceptROIselect(0);
	emit selectedROI(0);
	break;
      }
    }
    if (!selectedroi) {
      // no selection
      dbg("roiimage:blobroi: click w/o prior selection");
      int id = findNearestROI(clickPoint,0);
      if (id) {
	acceptROIselect(id);
	emit selectedROI(id);
	dbg("  -> selected %i",selectedroi);
      } else {
	// let's make a new one
	selectedroi = justcreatedroi = ensureIdNew();
	dbg("  -> creating new %i",selectedroi);
	PolyBlob &pb = roiset->newblob(selectedroi);
	ZoomInfo z = makeZoomInfo();
	visiblob->setZoom(z.ax,z.bx, z.ay,z.by); // make sure we have zoom right
	visiblob->show();
	visiblob->startCreate(&pb, e);
      }
    }
  } break;
  }
}

void ROIImage::selectNearestROI(QPoint xy, double margin) {
  int inroi = findNearestROI(xy,margin);
  if (inroi) {
    selectedroi = inroi;
    recalcEllipse();
    emit selectedROI(selectedroi);
  }
}

int ROIImage::findNearestROI(QPoint xy, double marg) {
  QPointF xy_ = screenToImage(xy);
  int besti=0;
  double dd=numbers.inf;
  QSet<int> const &ids = roiset->ids();
  for (QSet<int>::const_iterator i=ids.begin(); i!=ids.end(); i++) {
    int id = *i;
    double d = sq(xy_.x()-roiset->centerX(id)) + sq(xy_.y()-roiset->centerY(id));
    if (d<dd) {
      besti = id;
      dd = d;
    }
  }
  if (besti) 
    if (roiset->inside(besti, xy_.x(),xy_.y(),screenToImage(marg)))
      return besti;
  return 0;
}

void ROIImage::mouseReleaseEvent(QMouseEvent *e) {
  dbg("ROIImage: release (%i,%i) mode=%i\n",e->x(),e->y(),int(clickMode));
  if (clickMode==CM_Zoom || clickMode==CM_None) {
    CCDImage::mouseReleaseEvent(e);
  } else if (clickPoint.x()<0) {
    // this is the release of a double click
    dbg("roiimage: release from double click");
    if (justcreatedroi>0) { // this really ought not happen, but I think it does
      roiset->remove(justcreatedroi);
      selectedroi=0;
      recalcEllipse();
      emit selectedROI(0);
    }
  } else if (roiset->isXYRRA(selectedroi)) {
    switch (clickMode) {
    case CM_AddROI:
      ellipse->complete(e);
      roiset->set(selectedroi, screenToImage(ellipse->getShape()));
      if (roiset->get(selectedroi).R<3 ||
	  roiset->get(selectedroi).r<3) {
	// Creation of tiny ROIs is discouraged; they would become invisible.
	if (justcreatedroi>0) {
	  roiset->remove(justcreatedroi);
	  selectedroi = 0;
	  recalcEllipse();
	  emit selectedROI(0);
	} else {
	  deleteROI();
	}
      } else {
	emit editedROI(selectedroi);
	emit selectedROI(selectedroi);
      }
      showSelectedID();
      break;
    case CM_MoveROI: case CM_ResizeROI: case CM_RotateROI: case CM_RotSizeROI:
      ellipse->complete(e);
      roiset->set(selectedroi, screenToImage(ellipse->getShape()));
      showSelectedID();
      dbg("roiimage:emit editedroi(%i,...)",selectedroi);
      emit editedROI(selectedroi);
      break;
    case CM_DelROI:
      break;
    default:
      break;
    }      
  } else if (roiset->isPoly(selectedroi)) {
    switch (clickMode) {
    case CM_BlobROI: case CM_MoveROI:
      dbg("release blob selectedroi=%i ispoly=%i",selectedroi,
	  roiset->isPoly(selectedroi));
      if (roiset->isPoly(selectedroi)) {
	if (visiblob->complete(e)) {
	  dbg("roiimage:blobroi: complete %i",selectedroi);
	  double sx=0, sy=0;
	  PolyBlob const &pb = roiset->getp(selectedroi);
	  for (int i=0; i<pb.nPoints(); i++) {
	    sx += pb.x(i);
	    sy += pb.y(i);
	  }
	  double r = pb.greatestRadius();
	  dbg("  -> center=(%.1f,%.1f) calc: (%.1f,%.1f) r=%.2f",
	      pb.x0(),pb.y0(),sx/pb.nPoints(),sy/pb.nPoints(), r);
	  if (r<3) {
	    dbg("    -> radius so small, we'll drop it");
	    if (justcreatedroi>0) {
	      roiset->remove(justcreatedroi);
	      recalcEllipse();
	      selectedroi = 0;
	      emit selectedROI(0);
	    } else {
	      deleteROI();
	    }
	  } else {
	    acceptROIedit(selectedroi);
	    emit editedROI(selectedroi);
	    acceptROIselect(selectedroi);
	    emit selectedROI(selectedroi);
	  }
	} else {
	  dbg("roiimage:blobroi: nothing to complete (%i)",selectedroi);
	}
      }
      break;
    case CM_ResizeROI: case CM_RotateROI: case CM_RotSizeROI:
      break;
    case CM_DelROI:
      break;
    default:
      break;
    }
  }
  justcreatedroi = 0;
}

void ROIImage::acceptROIselect(int id) {
  selectedroi = roiset->contains(id) ? id : 0;
  recalcEllipse();
  update();
}

void ROIImage::acceptROIedit(int id) {
  if (id==selectedroi)
    recalcEllipse();
  else
    update();
}

void ROIImage::mouseMoveEvent(QMouseEvent *e) {
  //printf("ROIImage: move (%i,%i)\n",e->x(),e->y());
  switch (clickMode) {
  case CM_Zoom: case CM_None:
    CCDImage::mouseMoveEvent(e);
    break;
  case CM_SelectROI:
    break;
  case CM_AddROI:
    if (roiset->isXYRRA(selectedroi))
      ellipse->drag(e);
    break;
  case CM_MoveROI:
    if (roiset->isXYRRA(selectedroi))
      ellipse->drag(e);
    else if (roiset->isPoly(selectedroi))
      visiblob->drag(e);
    break;
  case CM_ResizeROI: case CM_RotateROI: case CM_RotSizeROI:
    if (roiset->isXYRRA(selectedroi))
      ellipse->drag(e);
    break;
  case CM_DelROI:
    break;
  case CM_BlobROI:
    if (roiset->isPoly(selectedroi))
      visiblob->drag(e);
    break;
  }
}

void ROIImage::mouseDoubleClickEvent(QMouseEvent *e) {
  //printf("ROIImage: double Click (%i,%i)\n",e->x(),e->y());
  clickPoint=QPoint(-1,-1);
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
  if (selectedroi && roiset->isXYRRA(selectedroi)) {
    ellipse->setShape(imageToScreen(roiset->get(selectedroi)));
    ellipse->show();
  } else {
    ellipse->hide();
  }
  if (selectedroi && roiset->isPoly(selectedroi)) {
    visiblob->setShape(&(roiset->getp(selectedroi)), false);
    ZoomInfo z = makeZoomInfo();
    visiblob->setZoom(z.ax,z.bx, z.ay,z.by);
    visiblob->show();
  } else {
    visiblob->hide();
  }
}

void ROIImage::deleteROI() {
  deleteROI(selectedroi);
}

void ROIImage::deleteROI(int id) {
  if (roiset->contains(id)) {
    roiset->remove(id);
    acceptROIdelete(id);
    emit deletedROI(id);
  }
}

void ROIImage::acceptROIdelete(int id) {
  if (id==selectedroi) {
    acceptROIselect(0);
    emit selectedROI(0);
  }
}

void ROIImage::showROIs(ShowMode sm) {
  showMode = sm;
  update();
}

void ROIImage::showSelectedID(bool show) {
  selectedIDShown = show;
  if (showMode==SM_Full || showMode==SM_IDs)
    update();
}

void ROIImage::hideSelectedID() {
  showSelectedID(false);
  
}
