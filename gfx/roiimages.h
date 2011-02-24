// roiimages.h

#ifndef ROIIMAGES_H

#define ROIIMAGES_H

#include <QString>
#include <QMap>
#include <QRect>
#include <QSignalMapper>

#include <gfx/roiimage.h>

class ROIImages: public QObject {
  Q_OBJECT;
public:
  ROIImages();
  virtual ~ROIImages();
  void add(QString id, ROIImage *img);
public slots:
  void setMode(ROIImage::ClickMode cm);
  void showROIs(ROIImage::ShowMode sm);
  void setROIs(class ROISet *);
  void updateZoom(QRect); // rectangle in global coords
signals:
  void newZoom(QRect); // rectangle in global coords
private slots:
  void shareZoom(QString);
private:
  QMap<QString, ROIImage *> imgs;
  QSignalMapper *sm;
};

#endif
