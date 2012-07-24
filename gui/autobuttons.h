// autobuttons.h

#ifndef AUTOBUTTONS_H

#define AUTOBUTTONS_H

#include <gui/pagebuildgeom.h>
#include <gui/guipage.h>
#include <QString>
#include <QMap>
#include <QSet>
#include <xml/enumerator.h>
#include <gui/guibuttongroup.h>

class AutoButtons: public QObject {
  Q_OBJECT;
public:
  AutoButtons(guiButtonGroup *parent);
  void setup(PageBuildGeom &geom,
	     QDomElement doc);
  void rebuild(PageBuildGeom *g_out=0);
  QStringList selectIDs(QStringList inlist);
  bool isDynamic() const;
private:
  guiButtonGroup *parent_;
  QStringList ids;
  QMap<QString, guiButton *> buttons;
  QDomElement doc;
  PageBuildGeom initialGeom;
  Enumerator const *enumerator;
  bool isDyn;
};

#endif
