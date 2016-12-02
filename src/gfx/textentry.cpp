// textentry.cpp

#include <gfx/textentry.h>
#include <gfx/lineedit.h>
#include <gfx/virtkeyboard.h>
#include <QLabel>
#include <QPushButton>
#include <base/dbg.h>

const int TE_WIDTH = 320;
const int TE_HEIGHT = 128;

TextEntry::TextEntry(QWidget *parent): QFrame(parent) {
  kb = 0;
  QSize ps = parent->size();
  Dbg() << "TextEntry: " << ps.width() << "x" << ps.height();
  setGeometry(ps.width()/2-TE_WIDTH/2,
	      ps.height()/2-TE_HEIGHT,
	      TE_WIDTH,
	      TE_HEIGHT);
  setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
  setLineWidth(4);
  setAutoFillBackground( true );
  QPalette p;
  p = palette();
  p.setColor(QPalette::Window,"#ffffff");
  setPalette(p);
  
  message = new QLabel(this);
  message->setGeometry(16,16,TE_WIDTH-32,24);
  message->setPalette(p);
  QFont f = message->font();
  f.setPixelSize(20);
  message->setFont(f);

  contents = new LineEdit(this);
  contents->setGeometry(32,50,TE_WIDTH-64,32);
  f = contents->font();
  f.setPixelSize(26);
  contents->setFont(f);
  p.setColor(QPalette::Base,"#dddddd");
  contents->setPalette(p);
  
  clearb = new QPushButton(this);
  clearb->setGeometry(8,TE_HEIGHT-28-8,56,28);
  clearb->setText("Clear");
  p=clearb->palette();
  p.setColor(QPalette::Button,"#bbeebb");
  clearb->setPalette(p);
  clearb->setFocusPolicy(Qt::NoFocus);

  cancelb = new QPushButton(this);
  cancelb->setGeometry(TE_WIDTH-56-8-56-8,TE_HEIGHT-28-8,56,28);
  cancelb->setText("Cancel");
  p=cancelb->palette();
  p.setColor(QPalette::Button,"#eebbbb");
  cancelb->setPalette(p);
  cancelb->setFocusPolicy(Qt::NoFocus);

  okb = new QPushButton(this);
  okb->setGeometry(TE_WIDTH-56-8,TE_HEIGHT-28-8,56,28);
  okb->setText("OK");
  p=okb->palette();
  p.setColor(QPalette::Button,"#bbbbee");
  okb->setPalette(p);
  okb->setFocusPolicy(Qt::NoFocus);

  connect(cancelb,SIGNAL(clicked()),this,SLOT(click_cancel()));
  connect(okb,SIGNAL(clicked()),this,SLOT(click_ok()));
  connect(clearb,SIGNAL(clicked()),this,SLOT(click_clear()));
  connect(contents,SIGNAL(returnPressed()),this,SLOT(click_ok()));
  connect(contents,SIGNAL(escapePressed()),this,SLOT(click_cancel()));
  connect(contents,SIGNAL(textEdited(QString)),this,SIGNAL(edited(QString)));
  autohide = false;

  kb = new VirtKeyboard(parent);
  kb->setGeometry(ps.width()/2-500,
		  ps.height()/2+20,
		  1000,
		  350);
  connect(kb,SIGNAL(keyPress(QChar)),this,SLOT(syntheticKeyPress(QChar)));
}

TextEntry::~TextEntry() {
  if (kb)
    delete kb;
}

QString TextEntry::text() const {
  return txt; // contents->text();
}

void TextEntry::setMessage(QString s) {
  message->setText(s);
}

void TextEntry::setText(QString s) {
  txt = s;
  contents->setText(s);
}

void TextEntry::click_ok() {
  if (!okb->isEnabled())
    return;
  txt = contents->text();
  emit accept(txt);
  if (autohide)
    hide();  
}

void TextEntry::click_clear() {
  contents->setText("");
}

void TextEntry::click_cancel() {
  contents->setText(txt);
  emit cancel();
  if (autohide)
    hide();  
}

void TextEntry::setAutoHide(bool b) {
  autohide = b;
}

void TextEntry::showEvent(class QShowEvent *) {
  contents->setFocus(Qt::PopupFocusReason);
  if (kb)
    kb->show();
}

void TextEntry::hideEvent(class QHideEvent *) {
  if (kb)
    kb->hide();
}

void TextEntry::enable_ok(bool b) {
  okb->setEnabled(b);
}

void TextEntry::syntheticKeyPress(QChar asc) {
  switch (asc.toAscii()) {
  case 8: // backspace
    contents->backspace();
    break;
  case 9: // tab
    // not handled right now
    break;
  case 10: case 13: // newline or return
    click_ok();
    break;
  case 27: // escape
    click_cancel();
    break;
  case 127: // delete
    contents->del();
    break;
  case 21: // left arrow
    contents->setCursorPosition(contents->cursorPosition()-1);
    break;
  case 22: // right arrow
    contents->setCursorPosition(contents->cursorPosition()+1);
    break;
  case 25: // home
    contents->home(false);
    break;
  case 26: // end
    contents->end(false);
    break;
  default:
    if (asc>=32 && asc<=126)
      contents->insert(asc);
    break;
  }
  contents->setFocus(Qt::PopupFocusReason);
}

bool TextEntry::ctrlHeld() const {
  return contents->ctrlHeld();
}

