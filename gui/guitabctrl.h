// guitabctrl.h

#ifndef GUITABCTRL_H

#define GUITABCTRL_H

#include <QObject>
#include <QDomElement>
#include <QSet>
#include <QString>

class guiPage;
class RadioGroup;
class PageBuildGeom;

class guiTabCtrl: public QObject {
  Q_OBJECT;
public:
  guiTabCtrl(guiPage *parent);
  virtual ~guiTabCtrl();
  void build(PageBuildGeom &g, QDomElement doc);
  bool mayResize();
  QString id() const { return id_; }
  QStringList childIDs() const;
  guiPage *parent() { return parent_; }
public slots:
  void rebuild();
  void add(class Button *b);
private:
  guiPage *parent_;
  RadioGroup *rg;
  QString id_;
  QSet<class AutoButtons *> autoButtons;
  QSet<QString> fixedids;
};

#endif
