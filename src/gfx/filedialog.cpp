// gfx/filedialog.cpp - This file is part of VScope.
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

// filedialog.cpp

#include "filedialog.h"

#include <gfx/scrollbuttonbar.h>
#include <gfx/filechooser.h>
#include <gfx/button.h>
#include <base/dbg.h>

#include <QLineEdit>

FileDialog::FileDialog(QWidget *parent,
		       FileChooser *alt_dirs,
		       FileChooser *alt_files): QFrame(parent) {
  pathbar = new ScrollButtonBar(this);
  connect(pathbar,SIGNAL(clicked(int,QString)),
	  this,SLOT(selectedPath(int,QString)));
  pathbar->setButtonBackground(QColor("#ddeedd"));

  if (alt_dirs) {
    dirs = alt_dirs;
    dirs->setParent(this);
  } else {
    dirs = new FileChooser(this);
  }
  connect(dirs,SIGNAL(clickedItem(QString)),
	  this,SLOT(selectedDir(QString)));
  dirs->setBackground("#ddeedd");

  if (alt_files) {
    files = alt_files;
    files->setParent(this);
  } else {
    files = new FileChooser(this);
  }
  connect(files,SIGNAL(clickedItem(QString)),
	  this,SLOT(selectedFile(QString)));
  files->setBackground("#ddddee");

  cancel = new Button(this);
  cancel->setText("Cancel");
  connect(cancel,SIGNAL(activated(QString,QString)),
	  this,SIGNAL(canceled()));

  ok = new Button(this);
  ok->setText("OK");
  connect(cancel,SIGNAL(activated(QString,QString)),
	  this,SLOT(clickedOK()));

  editor = new QLineEdit(this);
  connect(editor,SIGNAL(textEdited(QString)),
	  this,SLOT(edited(QString)));
  connect(editor,SIGNAL(returnPressed()),
	  this,SLOT(hitReturn()));
  
  extn = "";
  path = "";
  confirmFlag = false;
  mustExistFlag = false;
  selFile = "";

  setMinimumSize(160,120);
  ok->hide();
}

FileDialog::~FileDialog() {
}

void FileDialog::resizeEvent(class QResizeEvent *e) {
  QFrame::resizeEvent(e);
  relayout();
}

void FileDialog::relayout() {
  pathbar->setGeometry(2,2,width()-192-2-2-4,37);
  editor->setGeometry(width()-192-2,2,192,37);

  int nc_dir = dirs->countItems();
  int nc_fil = files->countItems();
  //  Dbg() << "filedialog dir="<<nc_dir<<" fil="<<nc_fil;
  int w_dir = 0;
  int x0_fil = 2;
  if (nc_fil<=0)
    w_dir = width()-4;
  else if (nc_dir<=0)
    w_dir = 0;
  else {
    double frc = nc_dir / (nc_dir+nc_fil+.0);
    if (frc<.2)
      frc=.2;
    else if (frc>.8)
      frc=.8;
    w_dir = width()*frc-8;
  }
  if (w_dir<=0) {
    w_dir=0;
    dirs->hide();
  } else {
    dirs->setGeometry(2,2+37+6,w_dir,height()-2-37-6-2);
    dirs->show();
    x0_fil = w_dir+6;
  }
  if (nc_fil<=0) {
    files->hide();
  } else {
    files->setGeometry(x0_fil,2+37+6,width()-x0_fil-2,height()-2-37-6-2);
    files->show();
  }
  
  if (confirmFlag) {
    ok->setGeometry(width()-2-72,height()-2-37,72,37);
    cancel->setGeometry(width()-2-72-4-80,height()-2-37,80,37);
  } else {
    cancel->setGeometry(width()-2-80,height()-2-37,80,37);
  }
}

void FileDialog::mustExist(bool flg) {
  mustExistFlag = flg;
}

void FileDialog::requireConfirm(bool flg) {
  if (flg == confirmFlag)
    return;
  
  confirmFlag = flg;
  if (flg)
    ok->show();
  else
    ok->hide();
  relayout();
}

void FileDialog::goDir(QString path0) {
  QRegExp dosdr("/[A-Z]:/");
  if (dosdr.indexIn(path0)==0) {
    dbg("dos path detected starting with /");
    path0 = path0.mid(1);
  }
  QDir dir(path0);
  path = dir.absolutePath();
  pathbar->setAsPath(path);
  dirs->populateDirs(dir);
  files->populateFiles(dir,extn,true);
  dbg("path = %s",qPrintable(path));
  relayout();
  emit changedDir(path);
}

void FileDialog::setExtn(QString extn0) {
  extn = extn0;
  QDir dir(path);
  files->populateFiles(dir,extn,true);
  relayout();
}

void FileDialog::selectedFile(QString fn) {
  if (confirmFlag) {
    selFile = fn;
  } else {
    QDir dir(path);
    QString realfn = dir.absolutePath() + "/" + fn;
    if (!extn.isEmpty())
      realfn+="."+extn;
    emit chosenFile(realfn);
  }
}

void FileDialog::edited(QString fn) {
  dirs->scrollTo(fn);
  files->scrollTo(fn);
  if (fn.endsWith("/")) {
    QDir dir(path);
    QString realfn = dir.absolutePath() + "/" + fn;
    if (!dir.exists(realfn)) {
      QString alt = fn.left(fn.size()-1);
      QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot,
				       QDir::Name|QDir::IgnoreCase);
      for (QStringList::iterator i=dirs.begin(); i!=dirs.end(); ++i) {
	if ((*i).startsWith(alt,Qt::CaseInsensitive)) {
	  realfn = dir.absolutePath() + "/" + *i;
	  break;
	}
      }
    }
    if (dir.exists(realfn)) {
      editor->setText("");
      goDir(realfn);
      return;
    } else {
      // issue warning of some sort?
      dbg("FileDialog: Dir '%s' does not exist (edited)",qPrintable(fn));
    }      
  }
}

void FileDialog::hitReturn() {
  QString fn = editor->text();
  QDir dir(path);
  QString realfn = dir.absolutePath() + "/" + fn;
  if (mustExistFlag) {
    if (!extn.isEmpty() && !realfn.endsWith("."+extn))
      realfn += "." + extn;
    if (!QFile::exists(realfn)) {
      QStringList filters;
      if (extn.isEmpty())
	filters << "*";
      else
	filters << ("*."+extn);
      QStringList fns = dir.entryList(filters,QDir::Files,
				      QDir::Name|QDir::IgnoreCase);
      int matchcount=0;
      for (QStringList::iterator i=fns.begin(); i!=fns.end(); ++i) {
	if ((*i).startsWith(fn,Qt::CaseInsensitive)) {
	  realfn = dir.absolutePath() + "/" + *i;
	  matchcount++;
	}
      }
      if (matchcount>1)
	realfn=".nonexist.";
    }
    if (!QFile::exists(realfn)) {
      dbg("FileDialog: File '%s' does not exist (hitReturn)",qPrintable(fn));
      return;
    }
  }  
  emit chosenFile(realfn);
}

void FileDialog::selectedDir(QString d) {
  selFile="";
  if (d.startsWith("/"))
    goDir(d);
  else
    goDir(path + "/" + d);
}

void FileDialog::clickedOK() {
  if (selFile.isEmpty())
    return;
  
  emit chosenFile(selFile);
  selFile=""; // this should have GUI implications
}

void FileDialog::selectedPath(int idx, QString) {
  goDir(pathbar->getAsPath(idx));
}

void FileDialog::synthKeyPress(QChar asc) {
  switch (asc.unicode()) {
  case 8: // backspace
    editor->backspace();
    break;
  case 9: // tab
    // not handled right now
    break;
  case 10: case 13: // newline or return
    hitReturn();
    break;
  case 27: // escape
    emit canceled();
    break;
  case 127: // delete
    editor->del();
    break;
  case 21: // left arrow
    editor->setCursorPosition(editor->cursorPosition()-1);
    break;
  case 22: // right arrow
    editor->setCursorPosition(editor->cursorPosition()+1);
    break;
  case 25: // home
    editor->home(false);
    break;
  case 26: // end
    editor->end(false);
    break;
  default:
    if (asc>=32 && asc<=126)
      editor->insert(asc);
    break;
  }
  editor->setFocus(Qt::PopupFocusReason);
}
  
