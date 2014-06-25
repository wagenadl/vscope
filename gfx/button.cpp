// button.cpp

#include <gfx/button.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <base/minmax.h>

#include <QPainter>
#include <QEvent>

// Defaults for Button; can be overridden after construction.
int BUTTON_FontSize = 14;
char const *BUTTON_FontFamily = "Arial";
int BUTTON_Width = 80;
int BUTTON_Height = 27;
char const *BUTTON_Foreground = "black";
char const *BUTTON_Background = "#eeeeee";


//////////////////////////////// Button //////////////////////////////

Button::Button(QWidget *parent, int lx, int ty, QString myID):
  QLabel(parent), myID(myID) {
  isToggle=isRadio=false;
  isItem=false;
  isAction=true;
  isSelected=false;
  isEnabled_=true;
  vtype = VT_Label;
  setGeometry(lx,ty,BUTTON_Width,BUTTON_Height);
  setFocusPolicy(Qt::NoFocus);
  QFont f = font();
  f.setPixelSize(BUTTON_FontSize);
  f.setFamily(BUTTON_FontFamily);
  setFont(f);
  setFrameShape(QFrame::Panel);
  setLineWidth(1);
  setMidLineWidth(2);
  setBackground(QColor("#eeeeee"));
  setBackground(QColor(BUTTON_Background));
  setForeground(QColor(BUTTON_Foreground));
  setAutoFillBackground(true);
  setAlignment(Qt::AlignCenter);
  setWordWrap(true);
  setTextFormat(Qt::RichText);
  setMargin(1);
  makeReadOnly(false);
}

Button::~Button() {
}

void Button::makeReadOnly(bool ro) {
  readonly = ro;
  representState();
}

void Button::changeEvent(QEvent *e) {
  if (e->type()==QEvent::EnabledChange) {
    isEnabled_ = isEnabled();
    representState();
  }
  QLabel::changeEvent(e);
}

void Button::makeAction() {
  isToggle=isRadio=false;
  isAction=true;
  representState();
}

void Button::makeToggle() {
  isAction=isRadio=false;
  isToggle=true;
}

void Button::makeRadio() {
  isAction=isToggle=false;
  isRadio=true;
  representState();
}

void Button::makeItem() {
  isItem=true;
  representState();
}


void Button::setBackground(QColor const &bg) {
  QPalette p = palette();
  p.setColor(QPalette::Button,bg); // this is what our children use
  p.setColor(QPalette::Window,bg); // this is what our children use
  p.setColor(QPalette::Light,bg.lighter(200)); // this is upshadow
  p.setColor(QPalette::Midlight,QColor("#00ff00"));
  p.setColor(QPalette::Dark,bg.darker(300)); // this is for down shadow
  setPalette(p);
  representState();
}

void Button::setForeground(QColor const &fg) {
  QPalette p = palette();
  p.setColor(QPalette::Active,QPalette::ButtonText,fg);
  p.setColor(QPalette::Inactive,QPalette::ButtonText,fg);
  p.setColor(QPalette::Active,QPalette::WindowText,fg);
  p.setColor(QPalette::Inactive,QPalette::WindowText,fg);
  setPalette(p);
}

void Button::setSelected(bool sel) {
  bool oldState = isSelected;
  
  isSelected = sel;
  representState();

  if (isSelected!=oldState && !isAction) {
    if (isSelected)
      emit selected(myID,text());
    else 
      emit deselected(myID,text());
  }
}

void Button::toggleSelected() {
  if (isRadio)
    setSelected(true);
  else
    setSelected(!isSelected);
}

void Button::mousePressEvent(class QMouseEvent *) {
  setPressed();
  if (isAction) 
    emit activated(myID, text());
  else
    toggleSelected();
}

void Button::setPressed() {
  lastClick.start();
  setFrameShadow(QFrame::Sunken);
}

void Button::mouseReleaseEvent(class QMouseEvent *) {
  unsetPressed();
}

void Button::unsetPressed() {
  int dt = lastClick.elapsed();
  if (dt>=100)
    unsetPressedNow();
  else 
    QTimer::singleShot(100-dt, this, SLOT(unsetPressedNow()));
}

void Button::unsetPressedNow() {
  representState(); 
}  

void Button::mouseDoubleClickEvent(class QMouseEvent *) {
  setPressed();
  if (vtype==VT_Action) {
    setFrameShadow(QFrame::Sunken);
  }
  Dbg() << "Button::mouseDoubleClickEvent";
  emit doubleClicked(myID,text());
}

void Button::representState() {
  Dbg() << "representstate: " << myID << "=" << text() << ": " << vtype;
  QFont f = font();
  f.setBold(isSelected);
  setFont(f);
  QPalette p = palette();
  QColor bg = p.color(QPalette::Button);
  p.setColor(QPalette::Window, bg);
  //  QWidget *pw=0;
  setAutoFillBackground(true);
  switch (vtype) {
  case VT_Label:
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Raised);
    setLineWidth(1);
    break;
  case VT_PageOpen:
    setFrameShape(QFrame::Panel);
    setLineWidth(isSelected ? 0 : 1);
    setFrameShadow(QFrame::Raised);
    break;
  case VT_VarOpen:
    setFrameShape(QFrame::Panel);
    if (readonly && text().contains(":")) {
      representFlat(p);
    } else {
      setLineWidth(isSelected ? 0 : 1);
      setFrameShadow(QFrame::Raised);
    }
    break;
  case VT_VarValue:
    setFrameShape(QFrame::Panel);
    if (readonly) {
      representFlat(p);
      setDisabled(!isSelected);
    } else {
      setFrameShadow(isSelected ? QFrame::Sunken : QFrame::Raised);
      setLineWidth(isSelected ? 2 : 1);
      if (isSelected)
        p.setColor(QPalette::Window,   QColor(roundi(bg.red()*.5+255*.5),
                                              roundi(bg.green()*.5+255*.5),
                                              roundi(bg.blue()*.5)));
    }
    break;
  case VT_BooleanVar:
    setFrameShape(QFrame::Box);
    if (readonly) {
      representFlat(p);
      setFrameShadow(QFrame::Sunken);
    } else {
      setFrameShadow(QFrame::Sunken);
      setLineWidth(1);
      setMidLineWidth(3);
      p.setColor(QPalette::Mid,
                 isEnabled_
                 ? (isSelected ? QColor("#ff2200") : QColor("#884444"))
                 : QColor("#bbbbbb"));
    }
    break;
  case VT_PanelOpen:
    setFrameShape(QFrame::Panel);
    setFrameShadow(isSelected ? QFrame::Sunken : QFrame::Raised);
    setLineWidth(isSelected ? 3 : 1);
    if (isSelected)
      p.setColor(QPalette::Window, deeperColor(bg,2));
    p.setColor(QPalette::WindowText, isSelected
               ? QColor("#ffff00")
               : p.color(QPalette::ButtonText));
    setPalette(p);
    break;
  case VT_Action:
    setAutoFillBackground(false);
    setFrameShadow(QFrame::Raised);
    setLineWidth(0);
    break;
  case VT_Grouped:
    setFrameShape(QFrame::Panel);
    if (readonly) {
      representFlat(p);
      setDisabled(!isSelected);
    } else {
      setFrameShadow(isSelected ? QFrame::Sunken : QFrame::Raised);
      setLineWidth(isSelected ? 2 : 1);
      p.setColor(QPalette::WindowText,//isSelected
                 //? QColor("#ffff00") :
	       p.color(QPalette::ButtonText));
    }
    break;
  case VT_ArrayCtrl:
    if (readonly) {
      representFlat(p);
      setFrameShadow(QFrame::Sunken);
    } else {
      setFrameShape(QFrame::Box);
      setFrameShadow(QFrame::Sunken);
      setLineWidth(isSelected ? 0 : 1);
      setMidLineWidth(isSelected ? 4 : 3);
    }
    break; // Note: Color is handled by xmlPage
  }

  setPalette(p);
  //  if (isEnabled_)
  //    show();
  //  else
  //    hide();
}

void Button::setID(QString id) {
  myID = id;
}

void Button::setText(QString txt, bool noemit) {
  QString t = txt;
  t.replace(QRegExp("_([^ _]+)_?"), "<sub>\\1</sub>");
  QLabel::setText(t);
  if (!noemit)
    emit textChanged(myID, txt);
}

void Button::setVisualType(VISUALTYPE vt) {
  vtype = vt;
  representState();
}

void Button::paintEvent(class QPaintEvent *e) {
  if (vtype==VT_Action) {
    bool isSunken = frameShadow()==QFrame::Sunken;
    //    dbg("Button::paintEvent");
    QPainter p(this);
    //    dbg("Button::paintEvent: created painter");
    QRect re = rect();
    p.setBrush(palette().color(QPalette::Window));
    QPen pen(Qt::NoPen);
    p.setPen(pen);
    QPolygon poly(8);
    int l = re.left();
    int r = re.right();
    int w = re.width()/5;
    int t = re.top();
    int b = re.bottom();
    int h = re.height()/5;
    if (w<h)
      h=w;
    else
      w=h;
    poly.setPoint(0,l+w,t);
    poly.setPoint(1,r-w,t);
    poly.setPoint(2,r,t+h);
    poly.setPoint(3,r,b-h);
    poly.setPoint(4,r-w,b);
    poly.setPoint(5,l+w,b);
    poly.setPoint(6,l,b-h);
    poly.setPoint(7,l,t+h);
    p.drawPolygon(poly);

    poly = QPolygon(5);
    poly.setPoint(0,l,b-h);
    poly.setPoint(1,l,t+h);
    poly.setPoint(2,l+w,t);
    poly.setPoint(3,r-w,t);
    poly.setPoint(4,r,t+h);
    pen = QPen(palette().color(isSunken ? QPalette::Dark : QPalette::Light));
    p.setPen(pen);
    p.drawPolyline(poly);

    poly.setPoint(0,r,t+h);
    poly.setPoint(1,r,b-h);
    poly.setPoint(2,r-w,b);
    poly.setPoint(3,l+w,b);
    poly.setPoint(4,l,b-h);
    pen = QPen(palette().color(isSunken ? QPalette::Light : QPalette::Dark));
    p.setPen(pen);
    p.drawPolyline(poly);
    //    dbg("Button::paintEvent: done with painter");
    
  }
  QLabel::paintEvent(e);
}

void Button::representFlat(QPalette &p) {
  setFrameShadow(QFrame::Plain);
  setLineWidth(0);
  QPalette p0(parentWidget()->palette());
  p.setColor(QPalette::Window, p0.color(QPalette::Window));
}

QColor Button::mixColor(QColor const &a, QColor const &b, double afrac) {
  return QColor(int(a.red()*afrac + b.red()*(1-afrac)),
		int(a.green()*afrac + b.green()*(1-afrac)),
		int(a.blue()*afrac + b.blue()*(1-afrac)));
}

QColor Button::deeperColor(QColor const &a, double amount) {
  int h, s, v;
  a.getHsv(&h,&s,&v);
  s=255; // 2*s;
  if (s>255)
    s=255;
  v=int(v/amount);
  if (v>255)
    v=255;
  QColor c;
  c.setHsv(h,s,v);
  return c;
}

