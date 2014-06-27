// autoitems.h

#ifndef AUTOITEMS_H

#define AUTOITEMS_H

#include <gui/pagebuildgeom.h>
#include <gui/guipage.h>
#include <QString>
#include <QMap>
#include <QSet>
#include <xml/enumerator.h>

class AutoItems: public QObject {
  Q_OBJECT;
public:
  AutoItems(guiPage *parent);
  void setup(PageBuildGeom &geom,
	     QDomElement doc);
  void rebuild(PageBuildGeom *g_out=0);
  QStringList selectIDs(QStringList inlist);
private:
  QStringList ids;
  QMap<QString, guiButton *> items;
  QDomElement doc;
  PageBuildGeom initialGeom;
  Enumerator const *enumerator;
  bool immune;
};

#endif
