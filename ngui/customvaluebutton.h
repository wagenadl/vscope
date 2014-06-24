// customvaluebutton.h

#ifndef CUSTOMVALUEBUTTON_H

#define CUSTOMVALUEBUTTON_H

class CustomValueButton: public ValueButton {
  Q_OBJECT;
public:
  CustomValueButton(AbstractPage *parent);
  virtual ~CustomValueButton();
  virtual void setup(ParamTree *ptree, QXmlElement visdef);
  // the ptree must point to a leaf element; the visdef to the actual value
  virtual void retree(ParamTree *ptree);
};

#endif
