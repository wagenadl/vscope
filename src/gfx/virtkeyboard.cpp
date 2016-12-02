// virtkeyboard.cpp

#include "virtkeyboard.h"
#include <QPushButton>
#include <string.h>

VirtKeyboard::~VirtKeyboard() {
  for (int i=0; i<rows.size(); ++i)
    delete rows[i];
}

VirtKeyboard::VirtKeyboard(QWidget *parent): QFrame(parent) {
  shifted=capslock=false;
  char row1[]="`1234567890-=!!";
  char row1a[]="~!@#$%^&*()_+!";
  char row2[]="!qwertyuiop[]\\"; 
  char row2a[]="!QWERTYUIOP{}|";
  char row3[]="!asdfghjkl;'!";
  char row3a[]="!ASDFGHJKL:\"!";
  char row4[]="!zxcvbnm,./!";
  char row4a[]="!ZXCVBNM<>?!";
  double wleft[]={1,1.5,1.8,2.3};
  char *lower[] = { row1, row2, row3, row4 };
  char *upper[] = { row1a, row2a, row3a, row4a };
  for (int i=0; i<4; i++) {
    KeyRow *row = new KeyRow;
    rows.push_back(row);
    int J = strlen(lower[i]);
    double x = 0;
    for (int j=0; j<J; j++) {
      KeyCap cap;
      cap.widget = new QPushButton(this);
      cap.lower = lower[i][j];
      cap.upper = upper[i][j];
      cap.left = x;
      cap.width = (j==0) ? wleft[i] : (j==J-1) ? 15-x :
	(i==0 && j==J-2) ? 1.0 : 1; // The 1.0 is Backspaced. I'd like it to be wider, but Del has to go somewhere.
      x += cap.width;
      row->push_back(cap);
      if (lower[i][j]=='!') {
	switch (i) {
	case 0: // backspace or delete
	  if (j==J-1) {
	    cap.widget->setText("Del");
	    connect(cap.widget,SIGNAL(pressed()),
		    this,SLOT(clickedDelete()));
	  } else {
	    cap.widget->setText("Back\nSpace");
	    connect(cap.widget,SIGNAL(pressed()),
		    this,SLOT(clickedBackspace()));
	  }
	  break;
	case 1: // tab
	  cap.widget->setText("Tab");
	  connect(cap.widget,SIGNAL(pressed()),this,SLOT(clickedTab()));
	  break;
	case 2:
	  if (j==0) { // caps lock
	    cap.widget->setText("Caps\nLock");
	    cap.widget->setCheckable(true);
	    connect(cap.widget,SIGNAL(pressed()),
		    this,SLOT(clickedCapsLock()));
	    capskey=cap.widget;
	  } else { // enter
	    cap.widget->setText("Enter");
	    connect(cap.widget,SIGNAL(pressed()),
		    this,SLOT(clickedEnter()));
	  }
	  break;
	case 3: // shift
	  cap.widget->setText("Shift");
	  cap.widget->setCheckable(true);
	  connect(cap.widget,SIGNAL(pressed()),
		  this,SLOT(clickedShift()));
	  shiftkeys.push_back(cap.widget);
	  break;
	}
      } else {
	// regular keys
	connect(cap.widget,SIGNAL(pressed()),
		this,SLOT(clickedKey()));
	lowers[cap.widget]=lower[i][j];
	uppers[cap.widget]=upper[i][j];
	if (lower[i][j]>='a' && lower[i][j]<='z') 
	  // it's a letter
	  cap.widget->setText(QString("%1").arg(upper[i][j]));
	else if (lower[i][j]=='7')
	  cap.widget->setText("&&\n7");
	else
	  cap.widget->setText(QString("%1\n%2")
			      .arg(upper[i][j]).arg(lower[i][j]));
      }
    }
  }
  // create space bar and arrows
  KeyRow *row = new KeyRow;

  KeyCap esc;
  esc.left = 0;
  esc.width = 1;
  esc.widget = new QPushButton(this);
  esc.widget->setFocusPolicy(Qt::NoFocus);
  connect(esc.widget,SIGNAL(clicked(bool)),this,SLOT(clickedEscape()));
  esc.widget->setText("Esc");
  row->push_back(esc);
  
  
  KeyCap spc;
  spc.left=2.8; spc.width=8;
  spc.widget = new QPushButton(this);
  spc.widget->setFocusPolicy(Qt::NoFocus);
  connect(spc.widget,SIGNAL(clicked(bool)),this,SLOT(clickedSpace()));
  row->push_back(spc);

  char lftcap[]="<--";
  char rgtcap[]="-->";
  char homcap[]="Home";
  char endcap[]="End";
  char *speccaps[] = { homcap, lftcap, rgtcap, endcap };
  for (int j=0; j<4; j++) {
    KeyCap cap;
    cap.left = j + 11;
    cap.width = 1;
    cap.widget = new QPushButton(this);
    cap.widget->setFocusPolicy(Qt::NoFocus);
    connect(cap.widget,SIGNAL(clicked(bool)),this,j==0 ? SLOT(clickedHome()) : j==1 ? SLOT(clickedLeft()) : j==2 ? SLOT(clickedRight()) : SLOT(clickedEnd()));
    cap.widget->setText(speccaps[j]);
    row->push_back(cap);
  }
  rows.push_back(row);

  // I'd like to add an escape key; not sure where
  // I'd like to add a Del key to the right of back space
  // I'd like to add arrows below R. Shift.

  // connect shifts
  for (int i=0; i<shiftkeys.size(); i++) {
    for (int j=0; j<shiftkeys.size(); j++) {
      if (i!=j) {
	connect(shiftkeys[i],SIGNAL(clicked(bool)),
		shiftkeys[j],SLOT(setChecked(bool)));
      }
    }
  }
  setFocusPolicy(Qt::NoFocus);
  setAutoFillBackground(true);
  QPalette p = palette();
  p.setColor(QPalette::Button,"#eeeedd");
  p.setColor(QPalette::Window,"#888888");
  setPalette(p);
  relayout();
}

void VirtKeyboard::resizeEvent(class QResizeEvent *) {
  relayout();
}

void VirtKeyboard::relayout() {
  double h = height()-1;
  double w = width()-1;
  double hrow = h/5;
  double wkey = w/15;
  for (int i=0; i<5; i++) {
    KeyRow &row=*rows[i];
    int J = row.size();
    for (int j=0; j<J; j++) {
      int x = int(wkey*row[j].left);
      int y = int(i*hrow);
      int w = int(wkey * row[j].width); // : row[j+1].left-row[j].left));
      int h = int((i+1)*hrow) - y;
      row[j].widget->setGeometry(x+2,y+2,w-3,h-3);
    }
  }
}

void VirtKeyboard::clickedKey() {
  QObject *s = sender();
  if (shifted)
    emit keyPress(uppers[s]);
  else if (capslock) {
    if (uppers[s]>='A' && uppers[s]<='Z')
      emit keyPress(uppers[s]);
    else
      emit keyPress(lowers[s]);
  } else {
    emit keyPress(lowers[s]);
  }
  unshift();
}

void VirtKeyboard::unshift() {
  if (shifted) {
    for (int i=0; i<shiftkeys.size(); i++)
      shiftkeys[i]->setChecked(false);
    shifted=false;
  }
}

void VirtKeyboard::clickedCapsLock() {
  capslock = !capskey->isChecked();
  unshift();
}

void VirtKeyboard::clickedTab() {
  emit keyPress(9);
  unshift();
}

void VirtKeyboard::clickedEnter() {
  emit keyPress(10);
  unshift();
}

void VirtKeyboard::clickedSpace() {
  emit keyPress(32);
  unshift();
}

void VirtKeyboard::clickedBackspace() {
  emit keyPress(8);
  unshift();
}

void VirtKeyboard::clickedDelete() {
  emit keyPress(127);
  unshift();
}

void VirtKeyboard::clickedLeft() {
  emit keyPress(21);
  unshift();
}

void VirtKeyboard::clickedRight() {
  emit keyPress(22);
  unshift();
}

void VirtKeyboard::clickedHome() {
  emit keyPress(25);
  unshift();
}

void VirtKeyboard::clickedEnd() {
  emit keyPress(26);
  unshift();
}

void VirtKeyboard::clickedEscape() {
  emit keyPress(27);
  unshift();
}

void VirtKeyboard::clickedShift() {
  shifted = !shiftkeys[0]->isChecked();
}

