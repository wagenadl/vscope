// textedit.h

/* This class is essentially the "CodeEditor" example from the Qt 4.8
   documentation.
   © 2015 The Qt Company Ltd. Documentation contributions included
   herein are the copyrights of their respective owners. The
   documentation provided herein is licensed under the terms of the
   GNU Free Documentation License version 1.3 as published by the Free
   Software Foundation. Qt and respective logos are trademarks of The
   Qt Company Ltd. in Finland and/or other countries worldwide. All
   other trademarks are property of their respective owners.
*/

#ifndef TEXTEDIT_H

#define TEXTEDIT_H

#include <QPlainTextEdit>

class TextEdit: public QPlainTextEdit {
  Q_OBJECT;
public:
  TextEdit(QWidget *parent=0);
  virtual ~TextEdit();
  void repaintLineNumberArea(QPaintEvent *);
  int lineNumberAreaWidth();
protected:
  void resizeEvent(QResizeEvent *event);
private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void updateLineNumberArea(const QRect &, int);
private:
  QWidget *lineNumberArea;
};

#endif
