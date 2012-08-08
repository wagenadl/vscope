// guitabctrl.h

#ifndef GUITABCTRL_H

#define GUITABCTRL_H

#include <QObject>
#include <QDomElement>
#include <QSet>
#include <QString>
#include <gui/guiradiogroup.h>

class guiPage;
class RadioGroup;
class PageBuildGeom;

class guiTabCtrl: public guiRadioGroup {
  Q_OBJECT;
public:
  guiTabCtrl(guiPage *parent);
  virtual ~guiTabCtrl();
  void build(PageBuildGeom &g, QDomElement doc);
};

#endif
