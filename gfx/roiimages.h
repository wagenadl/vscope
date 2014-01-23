// roiimages.h

#ifndef ROIIMAGES_H

#define ROIIMAGES_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QRect>
#include <QSignalMapper>

#include <base/enums.h>
#include <gfx/roiimage.h>
#include <gfx/ccdimages.h>

class ROIImages: public CCDImages {
  Q_OBJECT;
public:
  ROIImages(QRect canvas);
  virtual ~ROIImages();
  ROIImage *get(QString id);
  ROIImage *first();
  void add(QString id, ROIImage *img);
public slots:
  void setMode(ROIImage::ClickMode cm);
  void showROIs(SHOWROIS sm);
  void setROIs(class ROISet *);
  void updateSelection(int);
signals:
  void newSelection(int);
private slots:
  void shareSelection(QString);
protected:
  QList<ROIImage *> images();
protected:
  QSignalMapper *sm;
};

#endif
