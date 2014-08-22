// floatimage.h

#ifndef FLOATIMAGE_H

#define FLOATIMAGE_H

#include <QVector>
#include <QImage>
#include <stdint.h>

class FloatImage: private QVector<float> {
public:
  FloatImage();
  FloatImage(uint16_t const *data, int width, int height);
  FloatImage(int width, int height);
  FloatImage(QImage const &);
  bool ensureSize(int width, int height);
  // clears and returns true if size is changing
  float operator()(int x, int y) const;
  float &operator()(int x, int y);
  float const *data() const;
  float *data();
  float const *row(int y) const;
  float *row(int y);
  int width() const;
  int height() const;
  FloatImage &operator=(QImage const &);
  FloatImage &operator+=(float);
  FloatImage &operator-=(float);
  FloatImage &operator*=(float);
  FloatImage &operator/=(float);
  FloatImage &operator+=(FloatImage const &);
  FloatImage &operator-=(FloatImage const &);
  FloatImage &operator*=(FloatImage const &);
  FloatImage &operator/=(FloatImage const &);
  FloatImage convNorm(FloatImage const &v,
                      class QProgressDialog *pd=0,
                      int v0=0, int v1=0) const;
  // can only convolve with odd*odd shaped image
  // also, v must be smaller than we are.
  FloatImage ace(float sx, int rx, float sy, int ry,
                 bool showProgress=false) const;
  void apply(float (*foo)(float));
  void noisify();
  double mean() const;
  QImage toImage(float min, float max) const;
  QImage toImage(float min, float max, QVector<uint8_t> const &lut) const;
  static QVector<uint8_t> sigmoidLUT(float alpha, int K=1024);
  static FloatImage gaussian(float sigmax, int rx, float sigmay=-1, int ry=-1);
private:
  bool ensureCompatible(FloatImage const &);
  static void grayPalette(QImage &);
private:
  int wid;
  int hei;
};

#endif
