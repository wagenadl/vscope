// mgauto.h

#ifndef MGAUTO_H

#define MGAUTO_H

#include <gfx/multigraph.h>
#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include <QDomElement>

class MGAuto: public MultiGraph {
  /*:C MGAuto
   *:D Multigraph set that reads from XML which channels to display
   */
  Q_OBJECT;
public:
  MGAuto(QWidget *parent, QString myname);
  /*:F constructor
   *:D Creates the set. 
  */
  virtual ~MGAuto();
public slots:
  void rebuild();
private:
  void newgraphs();
  void newtraces();
  bool useInputChannel(QString id) const;
  bool useOutputChannel(QString id) const;
private:
  QString myname;
  QList<QString> actual;
  QSet<QString> digiSet;
  QSet<QString> outputSet;
  QMap<QString, class TraceInfo *> traces;
};

#endif
