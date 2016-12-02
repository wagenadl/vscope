// qrect.cpp

#include <QRect>
#include <QRectF>
#include <base/transform.h>
#include <QDebug>

void report(QRect const &r) {
  qDebug() << r.left() << ":" << r.right()
	   << "  " << r.top() << ":" << r.bottom()
	   << "  " << r.width() << "x" << r.height();
}

void report(QRectF const &r) {
  qDebug() << r.left() << ":" << r.right()
	   << "  " << r.top() << ":" << r.bottom()
	   << "  " << r.width() << "x" << r.height();
}

void ints() {
  QRect r(100,200,50,75);
  Transform t(Transform::hreflection(200));
  report(t(r));
}

void flts() {
  QRectF r(100,200,50,75);
  Transform t(Transform::hreflection(200));
  report(t(r));
}

int main() {
  ints();
  qDebug();
  flts();
  return 0;
}
