// coherence.h

#ifndef COHERENCE_H

#define COHERENCE_H

#include <gfx/ccdimage.h>
#include <base/roidata.h>
#include <gfx/roiimage.h>
#include <toplevel/globals.h>
#include <toplevel/vsdtraces.h>
#include <math/cvec.h>
#include <QSet>
#include <base/ccdtiming.h>

class Coherence: public CCDImage {
  Q_OBJECT;
  /*:C Coherence
   *:N Really, this class ought to be constructed for a specific camera (pair),
       but the current version simply shows all ROIs over whatever image it
       is given.
       All coordinates are global.
   */
public:
  Coherence(class CohData *dat=0, QWidget *parent=0);
  virtual ~Coherence();
  virtual void showEvent(class QShowEvent *);
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the current image and any ROIs
   */
  class CohData /*const*/ *getData() const { return data; }
public slots:
  void setRefTrace(QString achn);
  void setRefDigi(int digiline);
  void setRefFreq(double fref_hz);
  void setShowMode(ROIImage::ShowMode sm);
private slots:
  void updateData();
private:
  void perhapsRefresh();
private:
  ROIImage::ShowMode showmode;
private:
  class CohData *data;
  bool owndata;
private: // these will not be implemented
  Coherence(Coherence const &other);
  Coherence &operator=(Coherence const &other);
};

#endif
