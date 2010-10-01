// filedlgkey.cpp

#include "filedlgkey.h"

#include <QLabel>
#include <gfx/exptfiledialog.h>
#include <gfx/virtkeyboard.h>
#include <gfx/button.h>
#include <base/dbg.h>

FileDlgKey::FileDlgKey(QWidget *parent, bool expts): QFrame(parent) {
  hdr = new QLabel(this);
  QFont f = hdr->font();
  f.setPixelSize(BUTTON_FontSize+2);
  f.setFamily(BUTTON_FontFamily);
  f.setBold(true);
  hdr->setFont(f);
  hdr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

  dbg("filedlgkey: expts=%i",expts);
  
  if (expts) 
    fd = new ExptFileDialog(this);
  else
    fd =  new FileDialog(this);
  
  connect(fd,SIGNAL(changedDir(QString)), this,SIGNAL(changedDir(QString)));
  connect(fd,SIGNAL(chosenFile(QString)), this,SIGNAL(chosenFile(QString)));
  connect(fd,SIGNAL(canceled()), this,SIGNAL(canceled()));

  kb = new VirtKeyboard(this);
  connect(kb,SIGNAL(keyPress(QChar)), fd, SLOT(synthKeyPress(QChar)));
}

FileDlgKey::~FileDlgKey() {
}

void FileDlgKey::mustExist(bool b) {
  fd->mustExist(b);
}

void FileDlgKey::requireConfirm(bool b) {
  fd->requireConfirm(b);
}

QString FileDlgKey::getPath() const {
  return fd->getPath();
}

void FileDlgKey::goDir(QString path) {
  fd->goDir(path);
}

void FileDlgKey::setExtn(QString extn) {
  fd->setExtn(extn);
}

void FileDlgKey::setLabel(QString s) {
  hdr->setText(s);
}

void FileDlgKey::resizeEvent(class QResizeEvent *e) {
  QFrame::resizeEvent(e);
  int w = width();
  int h = height();
  hdr->setGeometry(0,0,w,36);
  fd->setGeometry(0,40,w,h-w*.28-40);
  kb->setGeometry(20,h-w*.28+8,w-40,w*.28-10);
}
  
  
