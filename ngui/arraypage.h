// arraypage.h

#ifndef ARRAYPAGE_H

#define ARRAYPAGE_H

class ArrayPage: public ControlPage {
  Q_OBJECT;
public:
  ArrayPage();
  virtual ~ArrayPage();
  virtual void setup(ParamTree *tree, QDomElement visualdef);
  virtual void retree(ParamTree *tree);
};

#endif
