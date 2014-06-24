// valuebutton.h

#ifndef VALUEBUTTON_H

#define VALUEBUTTON_H

class ValueButton: public Button {
  Q_OBJECT;
public:
  ValueButton(AbstractPage *parent);
  virtual ~ValueButton();
  virtual void setup(ParamTree *ptree, QXmlElement visdef);
  // the ptree must point to a leaf element; the visdef to the actual value
  virtual void retree(ParamTree *ptree);
};

#endif
