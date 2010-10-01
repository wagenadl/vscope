// lineedit.h

#ifndef LINEEDIT_H

#define LINEEDIT_H

#include <QLineEdit>

class LineEdit: public QLineEdit {
  Q_OBJECT;
public:
  LineEdit(QWidget *parent);
  virtual ~LineEdit();
protected:
  virtual void keyPressEvent(class QKeyEvent *);
signals:
  void escapePressed(); // emitted when Escape is pressed
};

#endif
