// button.h

#ifndef BUTTON_H

#define BUTTON_H

class Button: public QAbstractButton {
  Q_OBJECT;
public:
  Button(AbstractPage *parent);
  virtual ~Button();
  virtual void setup(ParamTree *ptree, QXmlElement visdef);
  // the ptree may be null
  virtual void retree(ParamTree *ptree);
};

#endif
