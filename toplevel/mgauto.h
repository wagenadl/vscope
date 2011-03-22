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
  class Channel {
  public:
    Channel(QDomElement elt);
    static bool available(QString id, QString typ);
    /*:F available
     *:D Returns true iff this channel exists in the AICHAN/DIGILINES enums.
     */
  public:
    enum Type { AI, AO, DI, DO };
    QString id;
    QString label;
    int chn; // used for digital line, not for analog
    Type typ;
    bool tiny;
    bool isOut;
    bool isDigi;
    QString if_enabled;
  };
public:
  MGAuto(QWidget *parent, QDomElement conf, QString myname);
  /*:F constructor
   *:D Creates the set. CONF must be an element containing a <multigraph>
       element that has id=myname.
  */
  virtual ~MGAuto();
public slots:
  void rebuild();
private:
  void newgraphs();
  void newtraces();
private:
  QList<Channel> pool;
  QSet<QString> actual;
  QMap<QString, class TraceInfo *> traces;
};

#endif
