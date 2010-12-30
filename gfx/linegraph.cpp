// linegraph.cpp

#include <gfx/linegraph.h>

#include <base/numbers.h>
#include <base/minmax.h>
#include <math.h>

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QRect>
#include <QString>
#include <QVector>
#include <QRubberBand>
#include <base/dbg.h>
#include <QMouseEvent>

LineGraph::LineGraph(QWidget *parent): QFrame(parent),
				       axisPen(QColor("#888888")),
				       lastTracePen(QColor("black")),
				       backgroundColor("white"),
				       axisTextColor("#880000"),
				       axisTickLength(6),
				       axisMinorTickLength(2),
				       x0(-.2), x1(1.2),
				       y0(-.2), y1(1.2),
				       dx(0.), dxm(0.),
				       dy(0.), dym(0.),
				       labelHTickSpace(5),
				       labelVTickSpace(1),
				       hMargin(1),
				       vMargin(4),
				       autoRepaint(true),
				       xrangeFromAuto(true),
				       yrangeFromAuto(true),
				       xtickLabelsShown(true),
				       ytickLabelsShown(true) {
  //  dbg("LineGraph(%p)",this);
  rubberband = 0;
  setAutoFillBackground(true);
  QPalette p = palette();
  p.setColor(QPalette::Window, backgroundColor);
  setPalette(p);
  drawMode = Poly;
}

LineGraph::~LineGraph() {
  //  dbg("~LineGraph(%p)\n",this);
}

void LineGraph::setXRange(Range const &xx, bool nextAuto) {
  x0 = xx.min;
  x1 = xx.max;
  xrangeFromAuto = nextAuto;
  perhapsRepaint();
}

void LineGraph::setYRange(Range const &yy) {
  y0 = yy.min;
  y1 = yy.max;
  yrangeFromAuto = false;
  perhapsRepaint();
}

Range LineGraph::getXRange() const {
  return Range(x0,x1);
}

Range LineGraph::getYRange() const {
  return Range(y0,y1);
}


void LineGraph::perhapsRepaint() {
  if (autoRepaint)
    update();
}

void LineGraph::autoSetXRange() {
  Range xx = computeXRange();
  if (xx.empty()) {
    x0=0;
    x1=1;
  } else {    
    x0 = xx.min;
    x1 = xx.max;
  }
  double dx = x1-x0;
  x0 -= 1e-3*dx;
  x1 += 1e-3*dx;
  xrangeFromAuto = true;
  perhapsRepaint();
}

void LineGraph::autoSetYRange(double frc, double margfac) {
  Range yy = computeYRange(frc);
  if (yy.empty()) {
    y0=-1;
    y1=1;
  } else {
    y0 = yy.min;
    y1 = yy.max;
  }
  double dy = y1-y0;
  if (dy==0)
    dy=1;
  y0 -= margfac*dy;
  y1 += margfac*dy;
  yrangeFromAuto = true;
  perhapsRepaint();
}

void LineGraph::autoGrowXRange() {
  Range xx = computeXRange();
  xx.include(x0);
  xx.include(x1);
  x0 = xx.min;
  x1 = xx.max;
  xrangeFromAuto = true;
  perhapsRepaint();
}

void LineGraph::autoGrowYRange(double frc) {
  Range yy = computeYRange(frc);
  yy.include(y0);
  yy.include(y1);
  y0 = yy.min;
  y1 = yy.max;
  yrangeFromAuto = true;
  perhapsRepaint();
}

Range LineGraph::computeXRange() const {
  Range xx;
  for (Traces::const_iterator i=traces.constBegin();
       i!=traces.constEnd(); ++i) {
    TraceInfo const *ti = i.value();
    xx.include(ti->datax0);
    xx.include(ti->datax0+ti->datadx*ti->N);
  }
  return xx;
}

Range LineGraph::computeYRange(double frc) const {
  Range yy;
  for (Traces::const_iterator i=traces.constBegin();
       i!=traces.constEnd(); ++i) {
    TraceInfo const *ti = i.value();
    Range y1 = ti->zoomrange99(x0,x1,frc);
    yy.include(y1.min);
    yy.include(y1.max);
  }
  return yy;
}

void LineGraph::setXTicks(double dx0, double dxm0) {
  dx = dx0;
  dxm = dxm0;
  perhapsRepaint();
}

void LineGraph::setYTicks(double dy0, double dym0) {
  dy = dy0;
  dym = dym0;
  perhapsRepaint();
}

double LineGraph::getXTicks() const {
  if (dx>0)
    return dx;
  else
    return reasonableTick(x1-x0,contentsWidth());
}

double LineGraph::getYTicks() const {
  if (dy>0)
    return dy;
  else
    return reasonableTick(y1-y0,contentsHeight());
}

double LineGraph::getMinorXTicks() const {
  if (dxm>0)
    return dxm;
  else
    return reasonableMinorTick(x1-x0,contentsWidth());
}

double LineGraph::getMinorYTicks() const {
  if (dym>0)
    return dym;
  else
    return reasonableMinorTick(y1-y0,contentsHeight());
}

double LineGraph::reasonableTick(double dx, int fulpix) {
  double tick = (dx/5)*(1000.0/fulpix);
  if (tick>dx/2.05)
    tick=dx/2.05;
  double ord = pow(10,floor(log10(tick)));
  double digit = tick/ord;
  if (digit>=5)
    digit = 5;
  else if (digit>=2)
    digit = 2;
  else
    digit = 1;
  double res = ord*digit;
  //fprintf(stderr,"reasonableTick dx=%g fulpix=%i tick=%g res=%g\n",dx,fulpix,tick,res);
  return res;
}

double LineGraph::reasonableMinorTick(double dx, int fulpix) {
  double tick = reasonableTick(dx,fulpix);
  double ord = pow(10,floor(log10(tick)));
  double digit = tick/ord;
  if (digit>=5)
    digit = 1; // for major=5
  else if (digit>=2)
    digit = 1; // for major=2
  else
    digit = .5; // for major=1
  double res = ord*digit;
  if (res * fulpix/dx < 5)
    res=0;
  //fprintf(stderr,"reasonableMinorTick dx=%g fulpix=%i tick=%g res=%g\n",dx,fulpix,tick,res);
  return res;
}

void LineGraph::setAxisFont(QFont const &f) {
  axisFont = f;
  perhapsRepaint();
}

void LineGraph::setTraceFont(QFont const &f) {
  traceFont = f;
  perhapsRepaint();
}

void LineGraph::setBackgroundColor(QColor const &c) {
  backgroundColor = c;
  QPalette p = palette();
  p.setColor(QPalette::Window, backgroundColor);
  setPalette(p);
}

void LineGraph::setAxisPen(QPen const &p) {
  axisPen = p;
  perhapsRepaint();
}

void LineGraph::setAxisTextColor(QColor const &c) {
  axisTextColor = c;
  perhapsRepaint();
}

void LineGraph::setAxisTickLength(int maj, int min) {
  axisTickLength = maj;
  axisMinorTickLength = min;
  perhapsRepaint();
}

double LineGraph::computeFirstTick(double x0, double x1, double dx,
				   int pix) {
  double scale = (x1-x0)/pix;
  scale=scale;
  double xx0 = x0; // first possible value for tick
  return dx*ceil(xx0/dx); // first actual value for tick (divisible by dx)
}

bool LineGraph::addTrace(QString id, TraceInfo const *ti) {
  bool hadAlready = traces.contains(id);
  traces[id] = ti;
  if (!hadAlready) 
    tracePens[id] = lastTracePen;
  perhapsRepaint();
  return hadAlready;
}

bool LineGraph::removeTrace(QString id) {
  bool r = traces.remove(id) > 0;
  perhapsRepaint();
  return r;
}

void LineGraph::removeAllTraces() {
  traces.clear();
  perhapsRepaint();
}

bool LineGraph::setTracePen(QString id, QPen const &p) {
  lastTracePen = p;
  tracePens[id] = p;
  perhapsRepaint();
  return traces.contains(id);
}

QPen LineGraph::getTracePen(QString id) const {
  if (tracePens.contains(id)) 
    return tracePens.value(id);
  else
    return lastTracePen;
}

bool LineGraph::setTraceLabel(QString id, QString lbl) {
  traceLabels[id] = lbl;
  perhapsRepaint();
  return traces.contains(id);
}

QString LineGraph::getTraceLabel(QString id) const {
  if (traceLabels.contains(id))
    return traceLabels.value(id);
  else
    return "";
}

void LineGraph::showXTickLabels(bool t) {
  xtickLabelsShown = t;
  perhapsRepaint();
}

void LineGraph::showYTickLabels(bool t) {
  ytickLabelsShown = t;
  perhapsRepaint();
}

double LineGraph::computeFirstXTick() {
  return computeFirstTick(x0,x1,getXTicks(), width());
}

double LineGraph::computeFirstYTick() {
  return computeFirstTick(y0,y1,getYTicks(), height());
}


double LineGraph::computeFirstMinorXTick() {
  return computeFirstTick(x0,x1,getMinorXTicks(), width());
}

double LineGraph::computeFirstMinorYTick() {
  return computeFirstTick(y0,y1,getMinorYTicks(), height());
}

int LineGraph::dataToScreenX(double x) const {
  return int((x-x0) * (contentsWidth()-2*hMargin) / (x1-x0))
    + contentsX0() + hMargin;
}

int LineGraph::dataToScreenY(double y) const {
  return int((y1-y) * (contentsHeight()-2*vMargin) / (y1-y0))
    + contentsY1() + vMargin;
}

double LineGraph::screenToDataX(int x) const {
  return (x-contentsX0()-hMargin) * (x1-x0) / (contentsWidth()-2*hMargin)
    + x0;
}

double LineGraph::screenToDataY(int y) const {
  return y1
    - (y-contentsY1()-vMargin) * (y1-y0) / (contentsHeight()-2*vMargin);
}

int LineGraph::contentsWidth() const {
  return contentsRect().width();
}

int LineGraph::contentsHeight() const {
  return contentsRect().height();
}

int LineGraph::contentsX0() const {
  return contentsRect().left(); //  - geometry().left();
}

int LineGraph::contentsY1() const {
  return contentsRect().top(); // - geometry().top();
}

void LineGraph::setXLabel(QString lbl) {
  xLabel = lbl;
}

void LineGraph::setYLabel(QString lbl) {
  yLabel = lbl;
}


void LineGraph::paintXAxis(QPainter &p) {
  p.setPen(axisPen);
  p.setFont(axisFont);
  int scry = height();

  if (axisMinorTickLength) {
    // draw minor x ticks
    double dx_ = getMinorXTicks();
    if (dx_>0) {
      for (double x = computeFirstMinorXTick(); x<x1; x+=dx_) {
	int scrx = dataToScreenX(x);
	p.drawLine(scrx,scry,scrx,scry-axisMinorTickLength-1);
      }
    }
  }

  // draw x-axis label
  QRect r = QRect(contentsX0(),0,
		  contentsWidth()-2,
		  scry-axisTickLength-labelVTickSpace);
  QRect br;
  if (xLabel.isEmpty()) {
    br = QRect(width(),height(),0,0);
  } else {
    br = p.boundingRect(r, Qt::AlignRight | Qt::AlignBottom, xLabel);
    p.setPen(axisTextColor);
    p.drawText(r, Qt::AlignRight | Qt::AlignBottom, xLabel);
    p.setPen(axisPen);
  }

  // draw major x ticks & labels
  double dx_ = getXTicks();
  int prec = reasonablePrecision(dx_);
  for (double x = computeFirstXTick(); x<=x1; x+=dx_) {
    int scrx = dataToScreenX(x);
    if (axisTickLength)
      p.drawLine(scrx,scry,scrx,scry-axisTickLength-1);
    if (xtickLabelsShown) {
      QString s = QString::number(double(x),'f',prec);
      QRect r1 = QRect(scrx-100,0,200,scry-axisTickLength-labelVTickSpace);
      QRect br1 = p.boundingRect(r1, Qt::AlignHCenter | Qt::AlignBottom, s);
      if (br1.right()<br.left()-2 && br1.left()>axisTickLength) {
	p.setPen(axisTextColor);
	p.drawText(r1, Qt::AlignHCenter | Qt::AlignBottom, s);
	p.setPen(axisPen);
      }
    }
  }
}

void LineGraph::paintYAxis(QPainter &p) {
  p.setPen(axisPen);
  p.setFont(axisFont);

  int scrx = contentsX0();

  if (axisMinorTickLength) {
    // draw minor y ticks
    double dy_ = getMinorYTicks();
    if (dy_>0) {
      for (double y = computeFirstMinorYTick(); y<y1; y+=dy_) {
	int scry = dataToScreenY(y);
	p.drawLine(scrx,scry,scrx+axisMinorTickLength,scry);
      }
    }
  }
  
  // draw y-axis label
  QRect r = QRect(scrx+axisTickLength+labelHTickSpace,
		  contentsY1()+1,100,100);
  QRect br;
  if (yLabel.isEmpty()) {
    br = QRect(0,0,0,0);
  } else {
    br = p.boundingRect(r, Qt::AlignLeft | Qt::AlignTop, yLabel);
    p.setPen(axisTextColor);
    p.drawText(r, Qt::AlignLeft | Qt::AlignTop, yLabel);
    p.setPen(axisPen);
  }

  // draw major y ticks & labels
  double dy_ = getYTicks();
  int prec = reasonablePrecision(dy_);
  for (double y = computeFirstYTick(); y<=y1; y+=dy_) {
    int scry = dataToScreenY(y);
    if (axisTickLength)
      p.drawLine(scrx,scry,scrx+axisTickLength,scry);
    if (ytickLabelsShown) {
      QString s = QString::number(double(y),'f',prec);
      QRect r1 = QRect(scrx+axisTickLength+labelHTickSpace,scry-100,100,200);
      QRect br1 = p.boundingRect(r1, Qt::AlignLeft | Qt::AlignVCenter, s);
      if (br1.top()>br.bottom() &&
	  br1.bottom()<contentsHeight()-0*axisTickLength) {
	p.setPen(axisTextColor);
	p.drawText(r1, Qt::AlignLeft | Qt::AlignVCenter, s);
	p.setPen(axisPen);
      }
    }
  }
}

void LineGraph::paintTrace(QPainter &p, TraceInfo const *ti) {

  int M = contentsWidth(); // number of pixels
  double dx_per_pix = (x1-x0) / M;
  if (dx_per_pix <= ti->getDX()) {
    // fewer than one data point per pixel
    int N = ti->getN();
    int x0pix = int(contentsX0()+(ti->getX0()-x0)/dx_per_pix);
    double dxpix = ti->getDX()/dx_per_pix;
    QVector<QPoint> pts(N);
    for (int n=0; n<N; n++) 
      pts[n] = QPoint(x0pix+int(dxpix*n), dataToScreenY(ti->getDatum(n)));
    p.drawPolyline(pts);
  } else {
    double *yy_min = new double[M];
    double *yy_max = new double[M];
    ti->trueBlue(x0,dx_per_pix,M, yy_min,yy_max);
  
    //dbg("linegraph(%p)::painttrace M=%i dxperpix=%g.\n",this,M,dx_per_pix);
    //dbg("  trace %p: x0=%g dx=%g dp=%p N=%i step=%i\n",
    //         ti,ti->datax0,ti->datadx,ti->dataptr.dp_none,ti->N,ti->step);
  
    double summin=0, summax=0;
    for (int m=0; m<M; m++) {
      summin+=yy_min[m];
      summax+=yy_max[m];
    }
    //dbg(" avgmin=%g avgmax=%g\n",summin/M,summax/M);
    
    int m0=0, m1=M;
    for (int m=0; m<M; m++) {
      if (yy_min[m]>yy_max[m]) {
        if (m0==m) {
  	m0++;
        } else {
  	m1=m;
  	break;
        }
      }
    }
    // dbg("linegraph: m0=%i m1=%i",m0,m1);
    M=m1-m0;
    if (drawMode==Poly) {
      // POLYGON DRAWING STYLE
      p.setBrush(QBrush(p.pen().color()));
      int x0pix = contentsX0();
      QPoint *pts = new QPoint[2*M];
      for (int m=0; m<M; m++) {
        pts[m].setX(x0pix+m+m0);
        pts[m].setY(dataToScreenY(yy_min[m+m0]));
        pts[2*M-m-1].setX(x0pix+m+m0);
        pts[2*M-m-1].setY(dataToScreenY(yy_max[m+m0]));
      }
      p.drawPolygon(pts,2*M);
      delete [] pts;
    } else {
      // LINES DRAWING STYLE
      QVector<QPoint> pts(2*M);
      int x0pix = contentsX0();
      for (int m=0; m<M; m++) {
        pts[2*m].setX(x0pix+m+m0);
        pts[2*m+1].setX(x0pix+m+m0);
        pts[2*m].setY(dataToScreenY(yy_min[m+m0]));
        pts[2*m+1].setY(dataToScreenY(yy_max[m+m0]));
      }
      p.drawLines(pts);
    }
    
    delete [] yy_min;
    delete [] yy_max;
  }
}

void LineGraph::paintEvent(class QPaintEvent *) {
  //  dbg("linegraph(%p)::paintevent\n",this);
  QPainter p(this);
  p.setFont(traceFont);
  int dy_legend = p.fontMetrics().lineSpacing();
  int y0_legend = p.fontMetrics().height() + 2;
  int y = y0_legend;
  for (Traces::iterator i=traces.begin(); i!=traces.end(); ++i) {
    p.setPen(tracePens[i.key()]);
    paintTrace(p, i.value());
    p.drawText(0,0,width()-2,y,
	       Qt::AlignRight|Qt::AlignBottom|Qt::TextSingleLine,
	       traceLabels[i.key()]);
    y += dy_legend;
  }
  paintXAxis(p);
  paintYAxis(p);
}

int LineGraph::reasonablePrecision(double dx) {
  double l = log10(dx);
  return ceili(max(0, 0.5-l));
}

void LineGraph::mousePressEvent(QMouseEvent *e) {
  clickPoint = e->pos();
  if (!rubberband)
    rubberband = new QRubberBand(QRubberBand::Rectangle, this);
  rubberband->setGeometry(e->x(),e->y(),0,0);
  rubberband->show();
}

void LineGraph::mouseMoveEvent(QMouseEvent *e) {
  if (!rubberband)
    return; // this shouldn't happen
  rubberband->setGeometry(QRect(clickPoint,e->pos()).normalized());
  QRect g = rubberband->geometry();
  //  dbg("rb %i-%i (%i)",g.left(),g.right(),g.width());
}

void LineGraph::mouseReleaseEvent(QMouseEvent *e) {
  if (clickPoint.x()<0)
    return; // it's a double click; we don't care
  if (!rubberband)
    return; // this should not happen
  rubberband->setGeometry(QRect(clickPoint,e->pos()).normalized());
  QRect r = rubberband->geometry();
  int x0pix, x1pix;
  if (r.width()<5 && r.height()<5) {
    // it's a click -> zoom in
    x0pix = clickPoint.x()-rect().width()/4;
    x1pix = clickPoint.x()+rect().width()/4;
  } else {
    x0pix = r.left();
    x1pix = r.right();
  }
  delete rubberband;
  rubberband=0;
  //  dbg("zoomreq x0=%i x1=%i",x0pix,x1pix);
  emit zoomRequest(Range(screenToDataX(x0pix),screenToDataX(x1pix)));
}

void LineGraph::mouseDoubleClickEvent(QMouseEvent *) {
  clickPoint = QPoint(-1,-1);
  emit zoomRequest(Range());
}

void LineGraph::setAutoRepaint(bool ar) {
  autoRepaint = ar;
}

void LineGraph::drawLines() {
  drawMode = Lines;
  update();
}

void LineGraph::drawPoly() {
  drawMode = Poly;
  update();
}
