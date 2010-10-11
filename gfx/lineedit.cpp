// lineedit.cpp

#include <gfx/lineedit.h>
#include <QKeyEvent>

LineEdit::LineEdit(QWidget *parent): QLineEdit(parent) {
}

LineEdit::~LineEdit() {
}

void LineEdit::keyPressEvent(QKeyEvent *ke) {
  //fprintf(stderr,"lineedit:keypressevent %i\n",ke->key());
  ctrlheld = ke->modifiers() & Qt::ControlModifier;
  switch (ke->key()) { 
  case Qt::Key_Escape: 
    emit escapePressed();
    return; 
  case Qt::Key_Enter:
  case Qt::Key_Return: 
    emit returnPressed();
    return; 
  default:
    QLineEdit::keyPressEvent(ke);
  }
}
