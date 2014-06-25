// controlpanel.h

#ifndef CONTROLPANEL_H

#define CONTROLPANEL_H

#include "xml/easyxml.h"
#include "controlpage.h"

class ControlPanel: public AbstractPage {
public:
  ControlPanel(ParamTree *ptree, EasyXML guidef, QWidget *parent);
  virtual ~ControlPanel();
  ControlPage *root() const { return rt; }
  virtual QRect childPageRect() const;
private:
  EasyXML guidef;
  ControlPage *rt;
};

#endif
