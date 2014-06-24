// abstractpage.h

#ifndef ABSTRACTPAGE_H

#define ABSTRACTPAGE_H

class AbstractPage: public QFrame {
  Q_OBJECT;
public:
  AbstractPage(AbstractPage *parent);
  AbstractPage(QWidget *parent);
  virtual ~AbstractPage();
  virtual void setup(ParamTree *tree, QDomElement visualdef);
  virtual void retree(ParamTree *tree);
};

#endif
