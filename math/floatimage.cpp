// floatimage.cpp

#include "floatimage.h"
#include <math.h>

FloatImage::FloatImage() {
  wid = hei = 0;
}

FloatImage::FloatImage(uint16_t const *src, int width, int height) {
  wid = width;
  hei = height;
  resize(wid*hei);
  float *dst = data();
  int n = wid*hei;
    while (n--) 
      *dst++ = *src++;
}

FloatImage::FloatImage(int width, int height) {
  wid = width;
  hei = height;
  resize(wid*hei);
}

FloatImage::FloatImage(QImage const &img) {
  wid = 0;
  hei = 0;
  *this = img;
}

bool FloatImage::ensureSize(int w, int h) {
  if (w==wid && h==hei)
    return false;
  wid = w; hei=h;
  resize(wid*hei);
  return true;
}

FloatImage &FloatImage::operator=(QImage const &img) {
  if (img.format()==QImage::Format_Indexed8) {
    wid = img.width();
    hei = img.height();
    resize(wid*hei);
    uint8_t const *src = img.bits();
    float *dst = data();
    int y = hei;
    while (y--) {
      int x = wid;
      while (x--) 
        *dst++ = *src++;
      src += img.bytesPerLine() - wid;
    }
  } else {
    *this = img.convertToFormat(QImage::Format_Indexed8);
  }
  return *this;
}


float FloatImage::operator()(int x, int y) const {
  Q_ASSERT(x>=0 && x<wid);
  return row(y)[x];
}


float &FloatImage::operator()(int x, int y) {
  Q_ASSERT(x>=0 && x<wid);
  return row(y)[x];
}


float const *FloatImage::data() const {
  return QVector<float>::data();
}


float *FloatImage::data() {
  return QVector<float>::data();
}


float const *FloatImage::row(int y) const {
  Q_ASSERT(y>=0 && y<hei);
  return data() + wid*y;
}


float *FloatImage::row(int y) {
  Q_ASSERT(y>=0 && y<hei);
  return data() + wid*y;
}


int FloatImage::width() const {
  return wid;
}

int FloatImage::height() const {
  return hei;
}

void FloatImage::apply(float (*foo)(float)) {
  int n = wid*hei;
  float *dst = data();
  while (n--) {
    *dst = foo(*dst);
    dst++;
  }
}

void FloatImage::noisify() {
  int n = wid*hei;
  float *dst = data();
  while (n--) 
    *dst++ += rand()/(RAND_MAX+1.0);
}
  

double FloatImage::mean() const {
  double a = 0;
  int n = wid*hei;
  float const *src = data();
  while (n--) 
    a += *src++;
  return a / (wid*hei);
}

FloatImage &FloatImage::operator+=(float v) {
  int n = wid*hei;
  float *dst = data();
  while (n--)
    *dst++ += v;
  return *this;
}


FloatImage &FloatImage::operator-=(float v) {
  int n = wid*hei;
  float *dst = data();
  while (n--)
    *dst++ -= v;
  return *this;
}


FloatImage &FloatImage::operator*=(float v) {
  int n = wid*hei;
  float *dst = data();
  while (n--)
    *dst++ *= v;
  return *this;
}


FloatImage &FloatImage::operator/=(float v) {
  int n = wid*hei;
  float *dst = data();
  while (n--)
    *dst++ /= v;
  return *this;
}


FloatImage &FloatImage::operator+=(FloatImage const &v) {
  if (ensureCompatible(v)) {
    int n = wid*hei;
    float const *src = v.data();
    float *dst = data();
    while (n--)
      *dst++ += *src++;
  }
  return *this;
}


FloatImage &FloatImage::operator-=(FloatImage const &v) {
  if (ensureCompatible(v)) {
    int n = wid*hei;
    float const *src = v.data();
    float *dst = data();
    while (n--)
      *dst++ -= *src++;
  }
  return *this;
}


FloatImage &FloatImage::operator*=(FloatImage const &v) {
  if (ensureCompatible(v)) {
    int n = wid*hei;
    float const *src = v.data();
    float *dst = data();
    while (n--)
      *dst++ *= *src++;
  }
  return *this;
}


FloatImage &FloatImage::operator/=(FloatImage const &v) {
  if (ensureCompatible(v)) {
    int n = wid*hei;
    float const *src = v.data();
    float *dst = data();
    while (n--)
      *dst++ /= *src++;
  }
  return *this;
}

FloatImage FloatImage::convNorm(FloatImage const &v) const {
  int w2 = v.width();
  int h2 = v.height();
  if ((w2&1)==0 || (h2&1)==0)
    return FloatImage();
  if (w2>=wid || h2>=hei)
    return FloatImage();
  int rx = (w2-1)/2;
  int ry = (h2-1)/2;

  FloatImage r(wid, hei);

  for (int y=0; y<ry; y++) {
    float const *src = row(0);
    float *dst = r.row(y);
    float const *s2_ = v.row(ry - y);
    for (int x=0; x<rx; x++) { // topleft corner
      float const *s2 = s2_ + rx - x;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<ry+1+y; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<rx+1+x; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
    }
    for (int x=rx; x<wid-rx; x++) { // top edge
      float const *s2 = s2_;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<ry+1+y; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<w2; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
      src++;
    }
    for (int x=wid-rx; x<wid; x++) { // top right corner
      float const *s2 = s2_;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<ry+1+y; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<rx+wid-x; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
      src++;
    }
  }
  
  for (int y=ry; y<hei-ry; y++) {
    float const *src = row(y-ry);
    float *dst = r.row(y);
    float const *s2_ = v.row(0);
    for (int x=0; x<rx; x++) { // left edge
      float const *s2 = s2_ + rx - x;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<h2; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<rx+1+x; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
    }
    
    for (int x=rx; x<wid-rx; x++) { // bulk
      float const *s2 = s2_;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<h2; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<w2; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
      src++;
    }

    for (int x=wid-rx; x<wid; x++) { // right edge
      float const *s2 = s2_;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<h2; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<rx+wid-x; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
      src++;
    }
  }

  for (int y=hei-ry; y<hei; y++) {
    float const *src = row(y-ry);
    float *dst = r.row(y);
    float const *s2_ = v.row(0);
    for (int x=0; x<rx; x++) { // bottom left corner
      float const *s2 = s2_ + rx - x;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<ry+hei-y; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<rx+1+x; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
    }

    for (int x=rx; x<wid-rx; x++) { // bottom edge
      float const *s2 = s2_;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<ry+hei-y; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<w2; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
      src++;
    }
    for (int x=wid-rx; x<wid; x++) { // bottom right corner
      float const *s2 = s2_;
      float a = 0;
      float b = 0;
      for (int dy=0; dy<ry+hei-y; dy++) {
        float const *sa = src + wid*dy;
        float const *sb = s2 + w2*dy;
        for (int dx=0; dx<rx+wid-x; dx++) {
          float q = *sb++;
          a += *sa++ * q;
          b += q;
        }
      }
      *dst++ = a/b;
      src++;
    }
  }
  
  return r;
}

void FloatImage::grayPalette(QImage &img) {
  img.setColorCount(256);
  for (int x=0; x<256; x++)
    img.setColor(x, qRgb(x, x, x));
}


QImage FloatImage::toImage(float min, float max) const {
  QImage img(wid, hei, QImage::Format_Indexed8);
  grayPalette(img);
  float const *src = data();
  uint8_t *dst = img.bits();
  float rng = max - min;
  int y = hei;
  while (y--) {
    int x = wid;
    while (x--) {
      float g = 256 * (*src++ - min) / rng;
      *dst++ = g<0 ? 0 : g>255 ? 255 : uint8_t(g);
    }
    dst += img.bytesPerLine() - wid;
  }
  return img;
}


QImage FloatImage::toImage(float min, float max,
                           QVector<uint8_t> const &lut) const {
  QImage img(wid, hei, QImage::Format_Indexed8);
  grayPalette(img);
  float const *src = data();
  uint8_t *dst = img.bits();
  float rng = max - min;
  uint8_t const *tbl = lut.data();
  int K = lut.size();
  int y = hei;
  while (y--) {
    int x = wid;
    while (x--) {
      float g = K * (*src++ - min) / rng;
      *dst++ = tbl[g<0 ? 0 : g>K-1 ? K-1 : int(g)];
    }
    dst += img.bytesPerLine() - wid;
  }
  return img;
}


QVector<uint8_t> FloatImage::sigmoidLUT(float alpha, int K) {
  QVector<uint8_t> lut(K);
  uint8_t *tbl;
  float y0 = tanh(-.5*alpha);
  float y1 = tanh(.5*alpha);
  for (int k=0; k<K; k++) {
    float g = alpha*(k/(K-1.0) - 0.5);
    *tbl++ = uint8_t((tanh(g) - y0) / (y1-y0) * 255.999);
  }
  return lut;
}

bool FloatImage::ensureCompatible(FloatImage const &v) {
  if (v.width() == wid && v.height() == hei)
    return true;

  *this = FloatImage();
  return false;
}

FloatImage FloatImage::gaussian(float sigx, int rx, float sigy, int ry) {
  if (sigy<0)
    sigy = sigx;
  if (ry<0)
    ry = rx;
  FloatImage f(2*rx+1, 2*ry+1);
  double sx2 = sigx*sigx;
  double sy2 = sigy*sigy;
  for (int y=-ry; y<=ry; y++) {
    float *dst = f.row(y+ry);
    for (int x=-rx; x<=rx; x++) 
      *dst++ = exp(-.5*(x*x/sx2 + y*y/sy2));
  }
  return f;
}

FloatImage FloatImage::ace(float sigx, int rx, float sigy, int ry) const {
  FloatImage g = gaussian(sigx, rx, sigy, ry);
  FloatImage avg = convNorm(g);
  FloatImage dif = *this; dif -= avg;
  FloatImage rms = dif; rms *= rms;
  rms = rms.convNorm(g);
  rms.apply(sqrtf);
  rms = rms.convNorm(g); // this may not really matter
  rms += rms.mean()/4;
  dif /= rms;
  return dif;
}
  
