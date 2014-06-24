// controlpage.h

#ifndef CONTROLPAGE_H

#define CONTROLPAGE_H

class ControlPage: public AbstractPage {
  Q_OBJECT;
public:
  ControlPage();
  virtual ~ControlPage();
  virtual void setup(ParamTree *tree, QDomElement visualdef);
  virtual void retree(ParamTree *tree);
};

#endif
