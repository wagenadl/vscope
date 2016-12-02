// ellipse.cpp

#include <gfx/ellipse.h>
#include <QPainter>
#include <QMouseEvent>
#include <math.h>
#include <base/numbers.h>
#include <base/solvep.h>
#include <base/dbg.h>
#include <base/transform.h>

double inline sq(double x) { return x*x; }

// Following rsin, arsin, ratan2 serve to convert from Qt's left-handed
// coordinate system to Matlab's right-handed coordinate system so that
// translation of ellipse routines is more straightforward.
inline double rsin(double a) { return -sin(a); }
inline double arsin(double y) { return asin(-y); }
inline double ratan2(double y, double x) { return atan2(-y,x); }

int Ellipse::nPoints = 32;

Ellipse::Ellipse(QWidget *parent): QWidget(parent) {
  xyrra.x0=0;
  xyrra.y0=0;
  xyrra.R=1;
  xyrra.r=1;
  xyrra.a=0;
  setEnabled(false); // we never want mouse or keyboard events
  mode = Nop;
  setGeometry(parent->rect());
}

Ellipse::~Ellipse() {
}

void Ellipse::setShape(XYRRA const &xyrra_) {
  xyrra = xyrra_;
  update();
}

XYRRA const &Ellipse::getShape() const {
  return xyrra;
}

void Ellipse::setPen(QPen const &p) {
  pen=p;
  update();
}

void Ellipse::paintEvent(class QPaintEvent *) {
  dbg("Ellipse(%p):paint. geom=(%ix%i+%i+%i)",this,
      geometry().width(),
      geometry().height(),
      geometry().x(),
      geometry().y());
  QPainter pntr(this);
  pntr.setPen(pen);
  xyrra.paint(&pntr,nPoints);
}

void Ellipse::startCreate(QMouseEvent *e) {
  startDrag(e,Create);
}

void Ellipse::startMove(QMouseEvent *e) {
  startDrag(e,Move);
}

void Ellipse::startResize(QMouseEvent *e) {
  omega = xyrra.findNearest(Transform::pixelCenter(e->pos()));
  theta = xyrra.parallel(omega) - Numbers::pi()/2;
  // dbg("ellipse:resize omega=%5.2g (%4.0f) theta=%5.2g (%4.0f)",omega,
  //     180*omega/Numbers::pi(),theta,180*theta/Numbers::pi());
  startDrag(e,Resize);
}

void Ellipse::startRotate(QMouseEvent *e, bool far0) {
  far=far0;
  omega = xyrra.findNearest(Transform::pixelCenter(e->pos()));
  startDrag(e,Rotate);
}

void Ellipse::startRotSize(QMouseEvent *e) {
  omega = xyrra.findNearest(Transform::pixelCenter(e->pos()));
  theta = xyrra.parallel(omega) - Numbers::pi()/2;
  // dbg("ellipse:rotsize omega=%5.2g (%4.0f) theta=%5.2g (%4.0f)",omega,
  //     180*omega/Numbers::pi(),theta,180*theta/Numbers::pi());
  startDrag(e,RotSize);
}

void Ellipse::startDrag(QMouseEvent *e, enum Mode m) {
  QPointF xy = Transform::pixelCenter(e->pos());
  x0 = xy.x();
  y0 = xy.y();
  xyrra0 = xyrra;
  mode = m;
  // dbg("ellipse:start %i:  [(%g,%g)+(%g,%g)/%g]",mode,
  //     xyrra.x0,xyrra.y0,xyrra.R,xyrra.r,xyrra.a);
  drag(e);
}

void Ellipse::complete(QMouseEvent *e) {
  drag(e);
  dbg("ellipse:complete: [(%g,%g)+(%g,%g)/%g]",
       xyrra.x0,xyrra.y0,xyrra.R,xyrra.r,xyrra.a);
}

void Ellipse::drag(QMouseEvent *e) {
  QPointF xy = Transform::pixelCenter(e->pos());
  double x1 = xy.x();
  double y1 = xy.y();
  switch (mode) {
  case Nop:
    break;
  case Create: { // translated from eldrag_new.m
    double x = (x0+x1)/2;
    double y = (y0+y1)/2;
    double a = fabs(x0-x1)/2;
    double b = fabs(y0-y1)/2;

    xyrra.x0 = x;
    xyrra.y0 = y;
    xyrra.R = fmax(a,b)*sqrt(2);
    xyrra.r = fmin(a,b)*sqrt(2);
    xyrra.a = (a>b) ? 0 : Numbers::pi()/2;
  } break;

  case Move: { // translated from eldrag_center.m
    double dx = x1-x0;
    double dy = y1-y0;
    
    xyrra.x0 = xyrra0.x0 + dx;
    xyrra.y0 = xyrra0.y0 + dy;
  } break;

  case Resize: { // translated from eldrag_resize.m
    xyrra = xyrra0;
    double dx = x1-x0;
    double dy = y1-y0;

    // Rotate to coord. sys. where near point is on far right of ellipse
    xyrra.a -= theta;
    double dX = cos(theta)*dx+rsin(theta)*dy;
    double dY = -rsin(theta)*dx + cos(theta)*dy;
    // NB: xyrra.x0 and xyrra.y0 have not been rotated

    // Find height in rotated coord sys
    double maxy = xyrra.maxY();
    double H = maxy - xyrra.y0;

    // Find moving point, rel. to center of ellipse
    double xi = xyrra.R*cos(omega);
    double eta = xyrra.r*rsin(omega);
    double X = cos(xyrra.a)*xi - rsin(xyrra.a)*eta;
    double Y = rsin(xyrra.a)*xi + cos(xyrra.a)*eta;

    // Move the point
    X += dX/2;
    Y += dY/2;
    H += dY/2;

    // Define a new ellipse based on maxy = H0, maxx=X, y(maxx)=Y
    if (H<-Y) 
      H = -Y + fabs(H+Y);

    double beta = 1/(H*H-Y*Y);
    double alpha = H*H/(X*X) * beta;
    double gamma = -Y/X * beta;

    // Convert back to radii.
    // Translated from elabc2rra.m
    double a = 0.5*ratan2(2*gamma, alpha-beta);
    double cs = cos(a);
    double sn = rsin(a);
    double R = 1/sqrt(alpha*cs*cs + beta*sn*sn + 2*gamma*cs*sn);
    double r = 1/sqrt(alpha*sn*sn + beta*cs*cs - 2*gamma*cs*sn);
    if (R<r) {
      double rr = r; r=R; R=rr;
      a = a + Numbers::pi()/2;
      if (a>=Numbers::pi())
	a -= 2*Numbers::pi();
    }
    // end of elabc2rra snippet
    xyrra.x0 += dx/2;
    xyrra.y0 += dy/2;
    xyrra.R = R;
    xyrra.r = r;
    xyrra.a = a + theta; // unrotate
  } break;
  case Rotate: {
    xyrra = xyrra0;
    // translated from eldrag_rotate.m
    double dx = x1-x0;
    double dy = y1-y0;

    double xi = xyrra.R*cos(omega);
    double eta = xyrra.r*rsin(omega);
    double cs = cos(xyrra.a);
    double sn = rsin(xyrra.a);
    double x = cs*xi-sn*eta;
    double y = sn*xi-cs*eta;
    if (far) {
      x*=2;
      y*=2;
    }
    double theta0 = ratan2(y,x);
    double theta1 = ratan2(y+dy,x+dx);
    double dphi = theta1-theta0;
    xyrra.a += dphi;
    if (far) {
      double x2 = x*cos(dphi)-y*rsin(dphi);
      double y2 = x*rsin(dphi)+y*cos(dphi);
      xyrra.x0 += x2/2 - x/2;
      xyrra.y0 += y2/2 - y/2;
    }
  } break;
  case RotSize: {
    xyrra = xyrra0;
    double dx = x1-x0;
    double dy = y1-y0;

    // rotate to coord. sys. where near point is on far right of ellipse:
    xyrra.a -= theta;
    double dX =  cos(theta)*dx + rsin(theta)*dy;
    double dY = -rsin(theta)*dx + cos(theta)*dy;
    // NB: el.x0, el.y0 have NOT been rotated
 
    // Find height (in rotated coord sys)
    double mxy = xyrra.maxY();
    double H = mxy-xyrra.y0;
 
    // Find moving point, relative to center of ellipse
    double xi = xyrra.R*cos(omega);
    double eta = xyrra.r*rsin(omega);
    double X = cos(xyrra.a)*xi - rsin(xyrra.a)*eta;
    double Y = rsin(xyrra.a)*xi + cos(xyrra.a)*eta;
 
    //  Find old angle and new angle of this point, and change in radius
    double theta0 = ratan2(Y,X);
    double theta1 = ratan2(2*Y+dY,2*X+dX);
    double dphi = theta1-theta0;
    double dR =  sqrt(sq(2*X)+sq(2*Y)) - sqrt(sq(2*X+dX)+sq(2*Y+dY));
    // dR has opposite sign from matlab code; I do not really understand why (2/11/09).
    
    //  Now, first do the rotation part of the transformation (back in base coords)
    xyrra.a += theta;
 
    // Find moving point again, relative to center of ellipse, now in base coords.
    xi = xyrra.R*cos(omega);
    eta = xyrra.r*rsin(omega);
    double x = cos(xyrra.a)*xi - rsin(xyrra.a)*eta;
    double y = rsin(xyrra.a)*xi + cos(xyrra.a)*eta;
    x1 = x*cos(dphi) - y*rsin(dphi);
    y1 = x*rsin(dphi) + y*cos(dphi);
    xyrra.a += dphi;
    xyrra.x0 += x1-x;
    xyrra.y0 += y1-y;

    // OK, now we are rotated, let's do the rescaling part
    // Once again, move our point to far right
    xyrra.a -= theta + dphi;

    // Move the point
    X += dR/2; // NB: Will later move x0,y0 some too.

    // Define a new ellipse based on maxy=H0, maxx=X, y(maxx)=Y.
    if (H<-Y)
      H = -Y + abs(H+Y);

    double beta = 1/(H*H-Y*Y);
    double alpha = (H*H)/(X*X) * beta;
    double gamma = -(Y/X) * beta;

    // Convert back to radii.
    // Translated from elabc2rra.m
    double a = 0.5*ratan2(2*gamma, alpha-beta);
    double cs = cos(a);
    double sn = rsin(a);
    double R = 1/sqrt(alpha*cs*cs + beta*sn*sn + 2*gamma*cs*sn);
    double r = 1/sqrt(alpha*sn*sn + beta*cs*cs - 2*gamma*cs*sn);
    // double a_ = a;
    if (R<r) {
      double rr = r; r=R; R=rr;
      a = a + Numbers::pi()/2;
      if (a>=Numbers::pi())
	a -= 2*Numbers::pi();
    }
    // end of elabc2rra snippet
    xyrra.R = R;
    xyrra.r = r;
    xyrra.a = a;

    xyrra.a += theta+dphi; // unrotate

    xyrra.x0 += dR/2*cos(theta+dphi);
    xyrra.y0 += dR/2*rsin(theta+dphi);
    // dbg("orig=(%.0f,%.0f)+(%.0f,%.0f)-%.2f new=(%.0f,%.0f)+(%.0f,%.0f)-%.2f H=%.0f th=%.2f dphi=%.2f a=%g b=%g c=%g a_=%.2f",
    //     xyrra0.x0,xyrra0.y0,xyrra0.R,xyrra0.r,xyrra0.a,
    //     xyrra.x0,xyrra.y0,xyrra.R,xyrra.r,xyrra.a,
    //     H,theta, dphi, alpha,beta,gamma,a_);
  } break;
  }
  // dbg("ellipse(%p):drag %i: [(%g,%g)+(%g,%g)/%g]",this,mode,
  //     xyrra.x0,xyrra.y0,xyrra.R,xyrra.r,xyrra.a);
  update();
}

   
