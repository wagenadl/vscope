// multigraph.cpp

#include "multigraph.h"
#include "linegraph.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <QSet>

QSet<MultiGraph *> MultiGraph::allofus;

MultiGraph::MultiGraph(QWidget *parent): QFrame(parent),
					 backgroundColor("#888888") {
  setAutoFillBackground(true);
  QPalette p = palette();
  p.setColor(QPalette::Window, backgroundColor);
  setPalette(p);
}

MultiGraph::~MultiGraph() {
  allofus.remove(this);
}

void MultiGraph::connectZooms() {
  for (QSet<MultiGraph*>::iterator i=allofus.begin(); i!=allofus.end(); ++i) {
    connect(*i,SIGNAL(shareZoom(Range)), this,SLOT(sharedZoomRequest(Range)));
    connect(this,SIGNAL(shareZoom(Range)), *i,SLOT(sharedZoomRequest(Range)));
  }
  allofus.insert(this);
}
  
void MultiGraph::setBackgroundColor(QColor const &bg) {
  backgroundColor = bg;
  QPalette p = palette();
  p.setColor(QPalette::Window, backgroundColor);
  setPalette(p);
}

void MultiGraph::addGraph(QString id, LineGraph *lg, bool tiny) {
  //  dbg("mg:add %s->%p",qPrintable(id),lg);
  bool found=false;
  for (int n=0; n<graphs.size(); n++) {
    if (ids[n]==id) {
      disconnect(graphs[n],SIGNAL(destroyed(QObject)),
		 this,SLOT(removeGraph(QObject *)));
      isTiny[n]=tiny;
      graphs[n]=lg;
      found = true;
    }
  }
  if (!found) {
    isTiny.push_back(tiny);
    graphs.push_back(lg);
    ids.push_back(id);
  }
  //dbg("  found=%i",found);
  connect(lg,SIGNAL(destroyed(QObject*)), this,SLOT(removeGraph(QObject *)));
  connect(lg,SIGNAL(zoomRequest(Range)),
	  this,SLOT(zoomRequest(Range)));
  
  if (isVisible())
    lg->show();
  newSize();
}

void MultiGraph::clear() {
  for (int n=0; n<graphs.size(); n++) {
    disconnect(graphs[n],SIGNAL(destroyed(QObject)),
	       this,SLOT(removeGraph(QObject *)));
  }
  isTiny.clear();
  graphs.clear();
  ids.clear();
}

void MultiGraph::deleteAll() {
  //  fprintf(stderr,"MultiGraph::deleteAll n=%i\n",graphs.size());
  while (graphs.size()>0) 
    delete graphs[0]; // causes call to removegraph through connection
}

void MultiGraph::removeGraph(QString id) {
  //  fprintf(stderr,"MultiGraph::removeGraph %s.\n",qPrintable(id));
  for (int n=0; n<graphs.size(); n++) {
    if (ids[n]==id) {
      isTiny.remove(n);
      disconnect(graphs[n],SIGNAL(destroyed(QObject)),
		 this,SLOT(removeGraph(QObject *)));
      graphs.remove(n);
      ids.remove(n);
      newSize();
    }
  }
}

void MultiGraph::removeGraph(QObject *lg) {
  //  fprintf(stderr,"MultiGraph::removeGraph %p.\n",lg);
  for (int n=0; n<graphs.size(); n++) {
    if ((QObject*)(graphs[n])==lg) {
      isTiny.remove(n);
      graphs.remove(n);
      ids.remove(n);
      newSize();
    }
  }
}

void MultiGraph::deleteGraph(QString id) {
  for (int n=0; n<graphs.size(); n++) {
    if (ids[n]==id) {
      delete graphs[n]; // causes call to removeGraph through connection
      return;
    }
  }
}

LineGraph *MultiGraph::findp(QString id) {
  for (int n=0; n<graphs.size(); n++) 
    if (ids[n]==id)
      return graphs[n];
  return 0;
}

LineGraph &MultiGraph::find(QString id) {
  LineGraph *lg = findp(id);
  if (lg)
    return *lg;
  throw Exception("MultiGraph","Graph '" + id + "' not found.","find");
}

void MultiGraph::paintEvent(class QPaintEvent *) {
  //  fprintf(stderr,"multigraph(%p)::paintevent\n",this);
  for (int i=0; i<graphs.size(); i++) {
    //dbg("  mg:paint %i %p",i,graphs[i]);
    graphs[i]->update();
  }
}

void MultiGraph::resizeEvent(class QResizeEvent *) {
  newSize();
}

void MultiGraph::newSize() {
  int wid=width();
  int hei=height();
  int ntotal=graphs.size();
  int ntiny=0;
  for (int n=0; n<ntotal; n++)
    if (isTiny[n])
      ntiny++;

  // Find out how many graphs have forces heights, and take those out
  // of the equation
  for (int n=0; n<graphs.size(); n++) {
    if (forcedHeights.contains(ids[n]) && !isTiny[n]) {
      ntotal--;
      hei-=forcedHeights[ids[n]];
    }
  }  

  // See how many pixels we have if tinies take up half of normals:
  double pixpergraph = hei/(ntotal-ntiny/2.0+1e-40);
  double pixpertiny = pixpergraph/2;
  if (pixpertiny>MAXTINYHEIGHT) {
    pixpertiny = MAXTINYHEIGHT;
    pixpergraph = (hei-ntiny*pixpertiny) / (ntotal-ntiny+1e-40);
  }

  if (ntiny==ntotal)
    pixpertiny = hei/(ntotal+1e-40);
  int y=0;
  for (int n=0; n<graphs.size(); n++) {
    int h = isTiny[n] ? int(pixpertiny)
      : forcedHeights.contains(ids[n]) ? forcedHeights[ids[n]]
      : int(pixpergraph);
    graphs[n]->setGeometry(1,y+1,wid-2,h-2);
    y+=h;
  }
}

void MultiGraph::zoomRequest(Range xx) {
  dbg("multigraph(%p): zoomrequest [%g-%g]",this,
      xx.min,xx.max);
  if (xx.empty()) {
    if (graphs.size()>0) {
      graphs[0]->autoSetXRange();
      xx = graphs[0]->getXRange();
    }
  }
  sharedZoomRequest(xx);
  emit shareZoom(xx);
}

void MultiGraph::sharedZoomRequest(Range xx) {
  dbg("multigraph(%p): sharedzoomrequest [%g-%g] ngraphs=%i",
      this,xx.min,xx.max,
      graphs.size());
  for (int n=0; n<graphs.size(); n++)
    if (xx.empty())
      graphs[n]->autoSetXRange();
    else
      graphs[n]->setXRange(xx);
}

void MultiGraph::setGraphHeight(QString id, int h) {
  forcedHeights[id] = h;
  newSize();
}

void MultiGraph::resetGraphHeight(QString id) {
  forcedHeights.remove(id);
  newSize();
}

void MultiGraph::drawPoly(QString id) {
  LineGraph *lg = findp(id);
  if (lg)
    lg->drawPoly();
}

void MultiGraph::drawLines(QString id) {
  LineGraph *lg = findp(id);
  if (lg)
    lg->drawLines();
}
