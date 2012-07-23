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
  void add(QString id);
  /*:F add
   *:D Adds a button to this group.
   */
  bool remove(QString id);
  /*:F remove
   *:D Removes a button from our group.
   *:R True if the button existed.
   *:N It is not an error if the button was never in the group.
   */
private:
  guiPage *parent;
  QString dfltButton;
  QString id;
  QStringList childids;
};

#endif
