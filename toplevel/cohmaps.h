// cohmaps.h

#ifndef COHMAPS_H

#define COHMAPS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QRect>
#include <QSignalMapper>

#include <base/enums.h>
#include <gfx/roiimage.h>

class CohMaps: public QObject {
  Q_OBJECT;
public:
  CohMaps(QRect canvas);
  virtual ~CohMaps();
  Coherence *get(QString id);
  Coherence *first();
  QStringList ids() const;
  void add(QString id, Coherence *img);
  void del(QString id);
  bool has(QString id) const;
public slots:
  void setRefTrace(QString achn);
  void setRefDigi(QString digiline);
  void setRefFreq(double fref_hz);
  void setShowMode(SHOWROIS sm);
  void updateSelection(int);
  void setCanvas(QRect const &r);
signals:
  void newSelection(int);
private slots:
  void shareSelection(QString);
private:
  QMap<QString, Coherence *> imgs;
  QSignalMapper *zm;
  QSignalMapper *sm;
  QRect canvas;
};

#endif
