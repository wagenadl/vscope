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
  QString id() const { return myid; }
  guiPage *parentPage() { return page; }
  bool mayResize();
public slots:
  void selectDefaultButton();
  virtual void add(class Button *b);
  virtual void rebuild();
private:
  QString myid;
  guiPage *page;
  RadioGroup *rg;
  QString dflt;
  QSet<class AutoButtons *> autoButtons;
  QSet<QString> fixedids;
};

#endif
