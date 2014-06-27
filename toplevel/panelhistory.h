// panelhistory.h

#ifndef PANELHISTORY_H

#define PANELHISTORY_H

#include <QString>
#include <QObject>
#include <xml/param.h>

class PanelHistory: public QObject {
  Q_OBJECT;
public:
  PanelHistory(class ParamTree *ptree, class guiRoot *gui);
  virtual ~PanelHistory();
  void setTree(ParamTree *);
  void makeButtons();
  void relabelAll();
  void newSelection(QString const &where);
  /*:F newSelection
   *:D User's selection on the given side (where is "Left" or "Right")
       has changed. We must find out what the new selection is, and
       show the corresponding item.
   */
  void oldSelection(QString const &p);
  /*:F oldSelection
   *:D User clicked on one of our own buttons. We need to show the
       corresponding item.
   */
private:
  void reprioritize(QString where, int n);
  /*:F reprioritize
   *:D Resets the priority of the item at the specified location to highest,
       so that it will not be a candidate for replacement any time soon.
   */
  QString itemAt(QString where, int n) const;
  /*:F itemAt
   *:D Returns the ID of the item available in the specified location.
       Note that this item may or may not be open.
   */
  QString itemAt(QString const &butname) const;
  void setItemAt(QString where, int n, QString what);
  int find(QString where, QString what) const;
  double prio(QString where, int n) const;
  void setPrio(QString where, int n, double prio);
  int weakest(QString where) const;
  void open(QString where, QString what);
  int countButtons() const;
  void makeButtons(QString where);
  void relabel(QString where);
private:  
  class guiButton &barButton(QString where, int n);
  /*:F barButton
   *:D barButton("Left",2) returns a reference to the button "panelLeft2".
   *:N This is a button in the top row.
   */
  class guiButton &menuButton(QString where, QString what);
  /*:F menuButton
   *:D menuButton("Left","VSDTraces") returns a reference to the button
       "panelLeft/VSDTraces".
   *:N This is a button in the menu.
   */
  Param const &whatPar(QString where, int n) const;
  Param &whatPar(QString where, int n);
  /*:F whatPar
   *:D whatPar("Left",2) returns the parameter "panelHistory/whatLeft2".
   */
  Param const &prioPar(QString where, int n) const;
  Param &prioPar(QString where, int n);
  /*:F prioPar
   *:D prioPar("Left",2) returns the parameter "panelHistory/prioLeft2".
   */
  QString niceLabel(QString where, QString what);
  /*:F niceLabel
   *:D whatName("Left","VSDTraces") returns "VSD\nTraces".
   */
  QString butNameToWhere(QString const &s) const;
  /*:F butNameToWhere
   *:D butNameToWhere("panelLeft2") returns "Left".
   */
  int butNameToNumber(QString const &s) const;
  /*:F butNameToNumber
   *:D butNameToNumber("panelLeft2") returns 2.
   */
  QString butNameToWhat(QString const &s) const;
  /*:F butNameToWhat
   *:D butNameToWhat("panelLeft2") returns the ID of the button currently
       shown in that location.
  */
  QWidget *childWidget(QString what);
  /*:F childWidget
   *:D Given the name of an item (e.g., "VSDTraces"), returns the widget
       that should be shown for that item.
   *:R Returns null if the item is unknown.
  */
private slots:
  void doubleClicked(QString id, QString txt);
private:
  bool busy;
  int nButtons;
  class QWidget *oldLeft, *oldRight;
private:
  QString lasttrial;
  int figno;
  ParamTree *ptree;
  guiRoot *gui;
};

#endif
