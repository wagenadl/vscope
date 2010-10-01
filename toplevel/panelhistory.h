// panelhistory.h

#ifndef PANELHISTORY_H

#define PANELHISTORY_H

#include <QString>

class PanelHistory {
public:
  PanelHistory();
  void makeButtons();
  void newSelection(QString const &where);
  void oldSelection(QString const &p);
private:
  void reprioritize(QString where, int n);
  QString shown(QString where, int n);
  void setShown(QString where, int n, QString what);
  int find(QString where, QString what);
  double prio(QString where, int n);
  void setPrio(QString where, int n, double prio);
  int weakest(QString where);
  void open(QString where, QString what);
  int countButtons();
  QString whatName(QString where, int n);
  QString prioName(QString where, int n);
  QString niceLabel(QString where, QString what);
  void makeButtons(QString where);
private:
  bool busy;
  int nButtons;
  class QWidget *oldLeft, *oldRight;
};

#endif
