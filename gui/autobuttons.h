// autobuttons.h

#ifndef AUTOBUTTONS_H

#define AUTOBUTTONS_H

#include <gui/pagebuildgeom.h>
#include <gui/guipage.h>
#include <QString>
#include <QMap>
#include <QSet>

class AutoButtons: public QObject {
  Q_OBJECT;
public:
  AutoButtons(guiPage *parent);
  void setup(PageBuildGeom const &geom,
	     QDomElement doc,
	     Enumerator const *enumerator);
  void rebuild();
private:
  QStringList ids;
  QMap<QString, guiButton *> buttons;
  QDomElement doc;
  PageBuildGeom initialGeom;
  Enumerator const *enumerator;
};

#endif
