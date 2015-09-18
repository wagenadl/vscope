// textedit.cpp

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

#include "textedit.h"
#include <QPainter>
#include <QTextBlock>

class LineNumberArea: public QWidget {
public:
  LineNumberArea(TextEdit *parent): QWidget(parent) {
    editor = parent;
  }
  QSize sizeHint() const {
    return QSize(editor->lineNumberAreaWidth(), 0);
  }
protected:
  void paintEvent(QPaintEvent *event) {
    editor->repaintLineNumberArea(event);
  }
private:
  TextEdit *editor;
};

TextEdit::TextEdit(QWidget *parent): QPlainTextEdit(parent) {
  lineNumberArea = new LineNumberArea(this);

  connect(this, SIGNAL(blockCountChanged(int)),
	  this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)),
	  this, SLOT(updateLineNumberArea(QRect,int)));

  updateLineNumberAreaWidth(0);
}

TextEdit::~TextEdit() {
}

int TextEdit::lineNumberAreaWidth() {
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }
  
  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
  
  return space;
}

void TextEdit::updateLineNumberAreaWidth(int /* newBlockCount */) {
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void TextEdit::updateLineNumberArea(const QRect &rect, int dy) {
  if (dy)
    lineNumberArea->scroll(0, dy);
  else
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}


void TextEdit::resizeEvent(QResizeEvent *e) {
  QPlainTextEdit::resizeEvent(e);
  
  QRect cr = contentsRect();
  lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
				    lineNumberAreaWidth(), cr.height()));
}

void TextEdit::repaintLineNumberArea(QPaintEvent *event) {
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), Qt::lightGray);
  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();
  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(0, top, lineNumberArea->width(),
		       fontMetrics().height(),
		       Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}
