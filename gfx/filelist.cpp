// filelist.cpp

#include "filelist.h"

#include <QDir>
#include <gfx/button.h>
#include <base/minmax.h>
#include <base/dbg.h>

FileList::FileList(QWidget *parent): QFrame(parent) {
  colwidth_pix = 128;
  rowheight_pix = 41;
  nrows = ncols = 0;
  bg=QColor("#dddddd");
}

FileList::~FileList() {
}

void FileList::setColumnWidth(int pix) {
  colwidth_pix = pix;
  relayout();
}

void FileList::setRowHeight(int pix) {
  rowheight_pix = pix;
  relayout();
}

void FileList::setHeight(int pix) {
  height_pix = pix;
  relayout();
}

void FileList::relayout() {
  int N = children.size();
  nrows = height_pix / rowheight_pix;
  if (nrows<1)
    nrows=1; // If only to avoid division by zero...
  ncols = ceili(double(N) / nrows);
  width_pix = colwidth_pix * ncols;
  if (height_pix!=height() || width_pix!=width())
    resize(width_pix, height_pix);
  int c=0;
  int r=0;
  for (int n=0; n<N; n++) {
    children[n]->setGeometry(2+colwidth_pix*c, 2+rowheight_pix*r,
			     colwidth_pix-4, rowheight_pix-4);
    r++;
    if (r>=nrows) {
      r=0;
      c++;
    }
  }
}

void FileList::populateFiles(QDir const &src, QString extn, bool hideext) {
  QStringList filters;
  if (extn.isEmpty())
    filters << "*";
  else
    filters << ("*."+extn);
  QStringList fns = src.entryList(filters,QDir::Files,QDir::Name|QDir::IgnoreCase);
  if (hideext)
    fns.replaceInStrings(QRegExp("\\."+extn+"$"), "");
  for (QStringList::iterator i=fns.begin(); i!=fns.end(); ) {
    if (includeFileHook(src,*i))
      ++i;
    else
      i = fns.erase(i);
  }
  populate(src, fns);
}

void FileList::populateDirs(QDir const &src) {
  QStringList dirs = src.entryList(QDir::AllDirs | QDir::NoDotAndDotDot,QDir::Name|QDir::IgnoreCase);
  //  dirs.push_front("[Up]");
  for (QStringList::iterator i=dirs.begin(); i!=dirs.end(); ) {
    if (includeFileHook(src,*i))
      ++i;
    else
      i = dirs.erase(i);
  }
  for (QStringList::iterator i=dirs.begin(); i!=dirs.end(); ++i)
    *i += "/";
  populate(src, dirs);
}

void FileList::populate(QDir const &src, QStringList fns) {
  int N = fns.size();
  int N0 = children.size();

  for (int n=0; n<N; n++) {
    if (n>=N0) {
      Button *b = new Button(this);
      children.push_back(b);
      b->setBackground(bg);
      b->setVisualType(Button::VTAction);
      b->show();
      connect(b,SIGNAL(activated(QString,QString)),
	      this,SLOT(buttonClicked(QString,QString)));
    }
    QString txt = fns[n];
    QString aux = infoHook(src, fns[n]);
    if (aux!="") {
      txt += "\n";
      txt += aux;
    }
    children[n]->setText(txt);
  }

  if (N<N0) {
    for (int n=N; n<N0; n++) 
      children[n]->deleteLater();
    children.remove(N,N0-N);
  }
  relayout();
}

void FileList::buttonClicked(QString, QString txt) {
  emit clickedItem(txt.replace(QRegExp("\n.*"),""));
}

int FileList::indexOf(QString partialfn) {
  int k=0;
  for (QVector<Button *>::iterator i=children.begin();
       i!=children.end(); ++i) 
    if ((*i)->text().startsWith(partialfn,Qt::CaseInsensitive))
      return k;
    else
      k++;

  return -1;
}

void FileList::setBackground(QColor bg0) {
  bg = bg0;
  for (QVector<Button *>::iterator i=children.begin();
       i!=children.end(); ++i) 
    (*i)->setBackground(bg);
}

bool FileList::includeDirHook(QDir const &, QString s) {
  return !s.startsWith("_");
}

bool FileList::includeFileHook(QDir const &, QString s) {
  return !s.startsWith("_");
}

QString FileList::infoHook(QDir const &, QString) {
  return "";
}
