// filechooser.cpp

#include "filechooser.h"
#include <gfx/filelist.h>
#include <gfx/button.h>
#include <gfx/hscrollbar.h>
#include <base/dbg.h>
#include <math.h>

#define H_BUTTON 38
#define DY_BUTTON 42

FileChooser::FileChooser(QWidget *parent, FileList *alt_child):
  QFrame(parent) {
  if (alt_child) {
    list = alt_child;
    list->setParent(this);
    dbg("filechooser %p accepts alt_child %p",this,alt_child);
  } else {
    list = new FileList(this);
  }
  connect(list,SIGNAL(clickedItem(QString)),
	  this,SLOT(clickedFile(QString)));

  //   buttonLeft = new Button(this);
  //   buttonLeft->setText("<--");
  //   connect(buttonLeft,SIGNAL(activated(QString,QString)),
  // 	  this,SLOT(clickedLeft()));
  //   
  //   buttonRight = new Button(this);
  //   buttonRight->setText("-->");
  //   connect(buttonRight,SIGNAL(activated(QString,QString)),
  // 	  this,SLOT(clickedRight()));
  
  //  buttonUp = new Button(this);
  //  buttonUp->setText("[Up]");
  //  connect(buttonUp,SIGNAL(activated(QString,QString)),
  //	  this,SLOT(clickedUp()));

  scrollBar = new HScrollBar(this);
  connect(scrollBar,SIGNAL(moved(double)),
	  this,SLOT(scroll(double)));
  connect(scrollBar,SIGNAL(moving(double)),
	  this,SLOT(scrolling(double)));

  //  setFrameStyle(QFrame::Panel|QFrame::Sunken);
  //  setLineWidth(1);
}

FileChooser::~FileChooser() {
}

void FileChooser::populateFiles(QDir const &src, QString extn, bool hideextn) {
  list->populateFiles(src,extn,hideextn);
  dbg("fc:popdirs");
  isDirs=false;
  relayout();
  scrollTo();
}

void FileChooser::populateDirs(QDir const &src) {
  list->populateDirs(src);
  dbg("fc:popdirs");
  isDirs=true;
  relayout();
  scrollTo();
}

void FileChooser::resizeEvent(class QResizeEvent *) {
  int w = width();
  int ncol = w/100; // a column must be at least 100 pix wide.
  if (ncol==0)
    ncol=1;
  int colw = w/ncol;
  list->setColumnWidth(colw);
  relayout();
}

int FileChooser::countColumns() const {
  return list->countColumns();
}

int FileChooser::countItems() const {
  return list->countItems();
}

void FileChooser::relayout() {
  int hei = height();
  int wid = width();

  bool needLeftRight = true;
  // try without Left/Right buttons; we'd have a little more space that way
  list->setHeight(hei);
  if (list->width() <= wid)
    needLeftRight = false;

  int y0; // this will be top for FileList
  
  // Up/Left/Right go on top for Dirs; Left/Right go below for Files.
  if (needLeftRight) {
    list->setHeight(hei-DY_BUTTON);
    if (isDirs) {
      // buttonUp->setGeometry(2,2,40,H_BUTTON);
      // buttonLeft->setGeometry(2+40+4,2,40,H_BUTTON);
      // buttonRight->setGeometry(2+2*40+2*4,2,40,H_BUTTON);
      // buttonUp->show();
      scrollBar->setGeometry(2,list->countRows()*list->rowHeight()+2,
			     wid-2-2,H_BUTTON);
      y0 = 0; //DY_BUTTON;
    } else {
      // buttonLeft->setGeometry(2,hei-H_BUTTON-2,40,H_BUTTON);
      // buttonRight->setGeometry(2+40+4,hei-H_BUTTON-2,40,H_BUTTON);
      // buttonUp->hide();
      scrollBar->setGeometry(2,list->countRows()*list->rowHeight()+2,
			     wid-2/*-80-4*/-2,H_BUTTON);
      y0 = 0;
    }      
    // buttonLeft->show();
    // buttonRight->show();
    scrollBar->show();
  } else {
    // buttonUp->hide();
    //buttonLeft->hide();
    //buttonRight->hide();
    scrollBar->hide();
    y0 = 0;
  }

  // make sure the FileList has a reasonable position
  int x0 = list->geometry().left();
  int w0 = list->geometry().width();
  //dbg("filechooser relayout listx0=%i,w0=%i  wid=%i",x0,w0,wid);
  if (x0+w0 < wid)
    x0 = wid-w0; // let's not look past the right edge of the list
  if (x0>0)
    x0 = 0; // let's not look past the left edge of the list
  list->move(x0,y0);
  scrollBar->setRange(0,list->countColumns());
  int nvis = wid/list->columnWidth();
  scrollBar->setSlider(-x0*list->countColumns()/(w0+.01), nvis);
}

void FileChooser::scroll(double x) {
  //  x = round(x);
  scrolling(x);
}

void FileChooser::scrolling(double x) {
  int x0 = int(-x*list->columnWidth());
  list->move(x0,0); // isDirs ? DY_BUTTON : 0);
}

void FileChooser::scrollTo(QString partialfn) {
  int idx = partialfn.isEmpty() ? (countItems()-1) : list->indexOf(partialfn);
  if (idx>=0) {
    int col = idx/list->countRows();
    int nvis = width()/list->columnWidth();
    if (col>=list->countColumns()-nvis)
      col=list->countColumns()-nvis;
    list->move(-col*list->columnWidth(),0);
    scrollBar->setSlider(col,nvis,true);
  }
}  

void FileChooser::clickedLeft() {
  int x0 = list->geometry().left();
  int y0 = list->geometry().top();
  int w0 = list->geometry().width();
  int dx = list->columnWidth();
  x0 += dx;
  if (x0+w0 < width())
    x0 = width()-w0; // let's not look past the right edge of the list
  if (x0>0)
    x0 = 0; // let's not look past the left edge of the list
  list->move(x0,y0);
}

void FileChooser::clickedRight() {
  int x0 = list->geometry().left();
  int y0 = list->geometry().top();
  int w0 = list->geometry().width();
  int dx = list->columnWidth();
  x0 -= dx;
  if (x0+w0 < width())
    x0 = width()-w0; // let's not look past the right edge of the list
  if (x0>0)
    x0 = 0; // let's not look past the left edge of the list
  list->move(x0,y0);
}

void FileChooser::clickedFile(QString fn) {
  if (fn=="[Up]")
    emit clickedItem("..");
  else
    emit clickedItem(fn);
}

void FileChooser::clickedUp() {
  emit clickedItem("..");
}

void FileChooser::setBackground(QColor bg) {
  list->setBackground(bg);
}

class Button *FileChooser::buttonp(QString fn) {
  return list->buttonp(fn);
}
