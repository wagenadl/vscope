// guiradiogroup.h

#ifndef GUIRADIOGROUP_H

#define GUIRADIOGROUP_H

#include <QObject>
#include <QDomElement>
#include <QString>
#include <QStringList>
#include <QSet>

class guiPage;
class RadioGroup;
class PageBuildGeom;

class guiRadioGroup: public QObject {
  Q_OBJECT;
public:
  guiRadioGroup(guiPage *parent);
  virtual ~guiRadioGroup();
  void build(PageBuildGeom &g, QDomElement doc);
  QStringList childIDs() const;
public slots:
  void selectDefaultButton();
private:
  guiPage *parent_;
  RadioGroup *rg;
  QString dflt;
  QSet<QString> ids;
};

#endif
