// actionbutton.h

#ifndef ACTIONBUTTON_H

#define ACTIONBUTTON_H

class ActionButton: public Button {
  Q_OBJECT;
public:
  ActionButton();
  virtual ~ActionButton();
  virtual void setup(ParamTree *ptree, QXmlElement visdef);
  virtual void retree(ParamTree *ptree);
};

#endif
