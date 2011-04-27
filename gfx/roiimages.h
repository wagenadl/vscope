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
  ROIImages(QRect canvas);
  virtual ~ROIImages();
  void add(QString id, ROIImage *img);
  ROIImage *get(QString id);
  ROIImage *first();
public slots:
  void setMode(ROIImage::ClickMode cm);
  void showROIs(ROIImage::ShowMode sm);
  void setCanvas(QRect const &r);
  QRect const &currentCanvas() const { return canvas; }
  void setROIs(class ROISet *);
  void updateZoom(QRect); // rectangle in global coords
  void updateSelection(int);
signals:
  void newZoom(QRect); // rectangle in global coords
  void newSelection(int);
private slots:
  void shareZoom(QString);
  void shareSelection(QString);
private:
  QMap<QString, ROIImage *> imgs;
  QSignalMapper *zm;
  QSignalMapper *sm;
  QRect canvas;
};

#endif
