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
#include <gfx/ccdimages.h>
#include <gfx/coherence.h>

class CohMaps: public CCDImages {
  Q_OBJECT;
public:
  CohMaps(QRect canvas);
  virtual ~CohMaps();
  Coherence *get(QString id);
  Coherence *first();
  void add(QString id, Coherence *img);
public slots:
  void setRefTrace(QString achn);
  void setRefDigi(QString digiline);
  void setRefFreq(double fref_hz);
  void setShowMode(SHOWROIS sm);
  void updateSelection(int);
signals:
  void newSelection(int);
private slots:
  void shareSelection(QString);
protected:
  QList<Coherence *> images();
protected:
  QSignalMapper *sm;
};

#endif
