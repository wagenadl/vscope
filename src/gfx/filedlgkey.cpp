// gfx/filedlgkey.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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

  //  dbg("filedlgkey: expts=%i",expts);
  
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
  
  
