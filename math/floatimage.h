// floatimage.h

#ifndef FLOATIMAGE_H

#define FLOATIMAGE_H

#include <QVector>
#include <QImage>
#include <stdint.h>

class FloatImage: private QVector<float> {
public:
  FloatImage();
  FloatImage(int width, int height);
  FloatImage(QImage const &);
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
  FloatImage convNorm(FloatImage const &v) const;
  // can only convolve with odd*odd shaped image
  // also, v must be smaller than we are
  void apply(float (*foo)(float));
  void noisify();
  double mean() const;
  QImage toImage(float min, float max) const;
  QImage toImage(float min, float max, QVector<uint8_t> const &lut) const;
  static QVector<uint8_t> sigmoidLUT(float alpha, int K=1024);
  static FloatImage gaussian(float sigma, int r);
private:
  bool ensureCompatible(FloatImage const &);
  static void grayPalette(QImage &);
private:
  int wid;
  int hei;
};

#endif
