// ccdimages.h

#ifndef CCDIMAGES_H

#define CCDIMAGES_H


#include <QString>
#include <QStringList>
#include <QMap>
#include <QRect>
#include <QSignalMapper>

#include <base/enums.h>
#include <gfx/ccdimage.h>

class CCDImages: public QObject {
  Q_OBJECT;
public:
  CCDImages(QRect canvas);
  virtual ~CCDImages();
  CCDImage *get(QString id);
  CCDImage *first();
  QStringList ids() const;
  void add(QString id, CCDImage *img);
  void del(QString id);
  bool has(QString id) const;
  QRect const &currentCanvas() const { return canvas; }
public slots:
  void setCanvas(QRect const &r);
protected:
  QMap<QString, CCDImage *> imgs;
  QRect canvas;
};

#endif
