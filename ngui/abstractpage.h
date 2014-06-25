// abstractpage.h

#ifndef ABSTRACTPAGE_H

#define ABSTRACTPAGE_H

class AbstractPage: public QFrame {
  Q_OBJECT;
public:
  AbstractPage(AbstractPage *parent);
  AbstractPage(QWidget *parent);
  virtual ~AbstractPage();
  virtual void retree(ParamTree *tree);
  virtual QRect childPageRect() const;
};

#endif
