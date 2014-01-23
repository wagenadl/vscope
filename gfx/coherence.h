// coherence.h

#ifndef COHERENCE_H

#define COHERENCE_H

#include <QSet>

#include <gfx/ccdimage.h>
#include <base/roidata.h>
#include <gfx/roiimage.h>
//#include <toplevel/globals.h>
#include <math/cvec.h>
#include <base/ccdtiming.h>
#include <base/enums.h>

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
  void setCamPair(class CamPair const &);
public slots:
  void setRefTrace(QString achn);
  void setRefDigi(QString digiline);
  void setRefFreq(double fref_hz);
  void setShowMode(SHOWROIS sm);
  void updateSelection(int);
signals:
  void newSelection(int);
protected:
  virtual void mouseReleaseEvent(class QMouseEvent *);
  /*:F mouseReleaseEvent
   *:D Response to mouse release depends on current click mode.
       If it is ZOOM, this ends dragging a zoom rectangle. If the rectangle
       is larger than nothing, we zoom in to the smallest containing
       square; otherwise, we simply zoom in 2x on the clicked point.
   *:N We take care to ignore release events that are associated with
       double clicks.
  */
private slots:
  void updateData();
private:
  void perhapsRefresh();
  int insideROI(QPoint xy);
  void select(int id);
private:
  SHOWROIS showmode;
private:
  class CohData *data;
  bool owndata;
  int selectedID;
  CamPair cp;
private: // these will not be implemented
  Coherence(Coherence const &other);
  Coherence &operator=(Coherence const &other);
};

#endif
