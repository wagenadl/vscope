// guiButtonGroup.h

#ifndef GUIBUTTONGROUP_H

#define GUIBUTTONGROUP_H

#include <QObject>
#include <QDomElement>
#include <gui/pagebuildgeom.h>
#include <gui/xmlpage.h>
#include <gui/xmlbutton.h>

class guiButtonGroup: public QObject {
  Q_OBJECT;  
public:
  guiButtonGroup(xmlPage *parent);
  void build(PageBuildGeom &g, QDomElement doc);
  virtual ~guiButtonGroup();
  QString groupId() const { return id; }
  QStringList const &childIDs() const { return childids; }
  void selectDefaultButton();
private:
  xmlPage *parent;
  QString dfltButton;
  QString id;
  QStringList childids;
};

#endif
