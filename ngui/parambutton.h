// parambutton.h

#ifndef PARAMBUTTON_H

#define PARAMBUTTON_H

class ParamButton: public Button {
  Q_OBJECT;
public:
  ParamButton(AbstractPage *parent);
  virtual ~ParamButton();
  virtual void setup(ParamTree *ptree, QXmlElement visdef);
  // the ptree must point to a leaf element
  virtual void retree(ParamTree *ptree);
};

#endif
