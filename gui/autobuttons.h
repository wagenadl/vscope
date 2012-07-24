// autobuttons.h

#ifndef AUTOBUTTONS_H

#define AUTOBUTTONS_H

#include <gui/pagebuildgeom.h>
#include <gui/guipage.h>
#include <QString>
#include <QMap>
#include <QSet>
#include <xml/enumerator.h>
#include <gui/guitabctrl.h>

class AutoButtons: public QObject {
  Q_OBJECT;
public:
  AutoButtons(guiTabCtrl *parent);
  void setup(PageBuildGeom &geom,
	     QDomElement doc);
  void rebuild(PageBuildGeom *g_out=0);
  QStringList selectIDs(QStringList inlist);
  bool isDynamic() const;
  QList<QString> childIDs() const;
private:
  guiTabCtrl *parent_;
  QStringList ids;
  QMap<QString, guiButton *> buttons;
  QDomElement doc;
  PageBuildGeom initialGeom;
  Enumerator const *enumerator;
  bool isDyn;
};

#endif
