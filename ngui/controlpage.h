// controlpage.h

#ifndef CONTROLPAGE_H

#define CONTROLPAGE_H

class ControlPage: public AbstractPage {
  Q_OBJECT;
public:
  ControlPage(ParamTree *tree, QDomElement visualdef, AbstractPage *parent);
  virtual ~ControlPage();
  virtual void setup
  virtual void retree(ParamTree *tree);
private:
  QList<ButtonLayout> lays;
};

#endif
