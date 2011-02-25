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

ROISet *ROIImage::dummyrois = new ROISet();

inline double sq(double x) { return x*x; }

inline double euclideanDist2(QPointF p1, QPointF p2) {
  return sq(p1.x()-p2.x()) + sq(p1.y()-p2.y());
}

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

void ROIImage::setCamPair(CamPair const &cp) {
  campair = cp;
}

ROISet *ROIImage::getROIs() const {
  return roiset;
}

void ROIImage::setROIs(ROISet *rs) {
  if (roiset!=dummyrois) {
    disconnect(roiset, SIGNAL(changed(int)), this, SLOT(update()));
    disconnect(roiset, SIGNAL(changedAll(int)), this, SLOT(update()));
  }
  
  roiset = rs ? rs : dummyrois;

  if (roiset!=dummyrois) {
    connect(roiset, SIGNAL(changed(int)), this, SLOT(update()));
    connect(roiset, SIGNAL(changedAll(int)), this, SLOT(update()));
  }
  
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
    CCDImage::zoomIn(roiset->get(selectedroi).center().toPoint());
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

  foreach (int id, roiset->ids()) {
    bool showID = showMode==SM_Full || showMode==SM_IDs;
    bool showDot = showMode==SM_Centers && !id==selectedroi;
    bool showOutline = showMode==SM_Full || showMode==SM_Outlines ||
      id==selectedroi;
    if (editing && id==selectedroi)
      showID = showDot = showOutline = false;

    QPointF xy0 = canvasToScreen()(roiset->get(id).center());
    if (showDot) 
      p.drawEllipse(xy0,2,2);

    if (showOutline) {
      ROICoords const &roi = roiset->get(id);
      if (roi.isXYRRA()) 
	roi.xyrra().transformed(canvasToScreen()).paint(&p);
      else if (roi.isBlob()) 
	roi.blob().paint(&p, canvasToScreen());
    }
    if (showID) {
      if (id==selectedroi)
	p.setPen(QColor("#ff0000"));
      p.drawText(QRectF(xy0.x()-1,xy0.y()-1,2,2),
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
  QPointF canvasPoint = canvasToScreen().inverse()(clickPoint);
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
    editing->xyrra() = XYRRA(canvasPoint);
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
    if (selectedroi && roiset->get(selectedroi).isXYRRA()) {
      editing = new ROICoords(roiset->get(selectedroi));
      ellipse->startResize(e);
    }
    break;
  case CM_RotateROI: 
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->get(selectedroi).isXYRRA()) {
      ellipse->startRotate(e);
    }
    break;
  case CM_RotSizeROI: 
    selectNearestROI(clickPoint,5);
    if (selectedroi && roiset->get(selectedroi).isXYRRA()) {
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
      if (roiset->get(selectedroi).isBlob()) {
	// We have a polyblob selection, so either we'll modify the blob,
	// or we'll deselect.
	PolyBlob const &blob = roiset->get(selectedroi).blob();
	QPointF xy = canvasToScreen().inverse()(clickPoint);
	double dr_center = blob.distToCenter(xy);
	double dr_edge = blob.distToEdge(xy);
	if (dr_center<dr_edge) {
	  // closer to center than to edge -> deselect
  	  select(0);
	  break; // we won't reselect!
	} else if (dr_center < blob.greatestRadius() + 2) {
	  // we're inside the max radius, so let's distort
	  editing = new ROICoords(blob);
	  visiblob->setTransform(canvasToScreen());
	  visiblob->show();
	  visiblob->startAdjust(e);
	} else {
	  // we're too far away, so let's deselect, and pos'bly reselect/create
	  select(0); // triggers following if
	}
      } else {
	// We have a non-polyblob selection, so let's just deselect
	// Not sure if this is the most obvious response
	select(0);
	break;
      }
    }
    if (!selectedroi) {
      // no selection
      int id = findNearestROI(clickPoint,0);
      select(id);
      if (!id) {
	// let's make a new one
	editing = new ROICoords();
	editing->makeBlob();
	visiblob->setTransform(canvasToScreen());
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
  Transform tinv = canvasToScreen().inverse();
  QPointF xy_ = tinv(xy);
  int bestid=0;
  double dd;
  foreach (int id, roiset->ids()) {
    double d = euclideanDist2(xy_, roiset->get(id).center());
    if (bestid==0 || d<dd) {
      bestid = id;
      dd = d;
    }
  }
  if (bestid) 
    if (roiset->get(bestid).inside(xy_, tinv.maplength(marg)))
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
      *editing = ellipse->getShape().transformed(canvasToScreen().inverse());
      toosmall = editing->xyrra().R < 3 || editing->xyrra().r < 3;
    } else if (editing->isBlob()) {
      visiblob->complete(e);
      toosmall = editing->blob().greatestRadius() < 3;
    }
    if (toosmall) {
      if (selectedroi)
	roiset->remove(selectedroi);
    } else {
      int id = selectedroi ? selectedroi : roiset->newROI(campair);
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
    ellipse->setShape(editing->xyrra().transformed(canvasToScreen()));
    ellipse->show();
  } else {
    ellipse->hide();
  }
  if (editing && editing->isBlob()) {
    visiblob->setShape(&editing->blob(), false);
    visiblob->setTransform(canvasToScreen());
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
