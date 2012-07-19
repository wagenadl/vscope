// guiButtonGroup.h

#ifndef GUIBUTTONGROUP_H

#define GUIBUTTONGROUP_H

#include <QObject>
#include <QDomElement>
#include <gui/pagebuildgeom.h>
#include <gui/guipage.h>
#include <gui/guibutton.h>

class guiButtonGroup: public QObject {
  Q_OBJECT;  
public:
  guiButtonGroup(guiPage *parent);
  void build(PageBuildGeom &g, QDomElement doc);
  virtual ~guiButtonGroup();
  QString groupId() const { return id; }
  QStringList const &childIDs() const { return childids; }
  void selectDefaultButton();
private:
  guiPage *parent;
  QString dfltButton;
  QString id;
  QStringList childids;
};

#endif
