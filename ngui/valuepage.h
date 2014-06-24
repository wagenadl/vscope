// valuepage.h

#ifndef VALUEPAGE_H

#define VALUEPAGE_H

class ValuePage: public AbstractPage {
  Q_OBJECT;
public:
  ValuePage();
  virtual ~ValuePage();
  virtual void setup(ParamTree *tree, QDomElement visualdef);
  virtual void retree(ParamTree *tree);
signals:
  void newValue(QString val);
};

#endif
