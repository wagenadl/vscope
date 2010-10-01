// xmlpage.cpp

#include <gui/xmlpage.h>

#include <QWidget>
#include <QButtonGroup>
#include <xml/paramtree.h>
#include <xml/attribute.h>
#include <gui/guigeom.h>
#include <gui/xmlgui.h>
#include <xml/enumerator.h>
#include <gui/xmlbutton.h>
#include <QRect>
#include <QMessageBox>
#include <QLabel>
#include <base/exception.h>
#include <base/dbg.h>
#include <gfx/buttongrouper.h>
#include <QPainter>
#include <QPen>

xmlPage::xmlPage(class QWidget *parent,
		 class ParamTree *ptree_,
		 QDomElement doc,
		 class xmlGui *master_,
		 QString mypath,
		 class QRect const &geom): QFrame(parent) {
  setLineWidth(0);
  neverOpened=true;
  setGeometry(geom);

  master = master_;
  ptree = origptree = ptree_;

  currentElement = "";

  myPath = mypath;
  myTag = doc.tagName();
  triID = "";

  QString bg = doc.attribute("bg");
  if (!bg.isEmpty()) {
    QPalette p=palette();
    p.setColor(QPalette::Window,QColor(bg));
    setPalette(p);
    setAutoFillBackground(true);
    setFrameStyle(QFrame::Panel | QFrame::Raised);
  } else {
    ;
  }
  if (doc.hasAttribute("bbg")) {
    QString bg = doc.attribute("bbg");
    QPalette p=palette();
    p.setColor(QPalette::Button,QColor(bg));
    setPalette(p);
  }    

  Geom g;
  bool ok;
  g.page.dxl = int(master->geom().buttondx*doc.attribute("subdxl")
		   .toDouble(&ok)); // default to 0.
  g.page.dxr = int(master->geom().buttondx*doc.attribute("subdxr")
		   .toDouble(&ok)); // default to 0.
  g.page.dyt = int(master->geom().buttondy*doc.attribute("subdyt")
		   .toDouble(&ok)); // default to 0.
  g.page.dyb = int(master->geom().buttondy*doc.attribute("subdyb")
		   .toDouble(&ok)); // default to 0.

  g.rows = doc.attribute("rows").toInt(&ok); // default to 0.
  g.cols = doc.attribute("cols").toInt(&ok); // default to 0.
  caption = doc.attribute("caption");
  caption = ""; // Current style is to not use captions
  g.caph = caption.isEmpty() ? master->geom().topy : master->geom().menucaph;
  g.button.dx = master->geom().buttondx;
  g.button.w = master->geom().buttonw;
  g.button.dy = master->geom().buttondy;
  g.button.h = master->geom().buttonh;
  g.button.y0 = g.caph;
  g.button.x0 = master->geom().leftx;
  if (g.rows>0) {
    if (geom.height()-g.caph<g.button.dy*g.rows) {
      g.button.dy = double(geom.height()
			   -g.caph-master->geom().bottomy+master->geom().inty)
	/ g.rows;
      g.button.h = int(g.button.dy) - master->geom().inty;
    } else {
      resize(width(),int(g.button.dy*(g.rows+1)-g.button.h));
    }
  }
  if (g.cols>0) {
    if (geom.width()<g.button.dx*g.cols) {
      g.button.dx = double(geom.width()-master->geom().leftx
			   -master->geom().rightx+master->geom().intx)
	/ g.cols;
      g.button.w = int(g.button.dx) - master->geom().intx;
    } else {
      // int w0 = width();
      resize(int(g.button.dx*(g.cols+1)-g.button.w),height());
      // dbg("g.page.dxl=%i dxr=%i",g.page.dxr,g.page.dxl);
      // if (g.page.dxr)
      //   move(geometry().left()+50/*w0-width()*/,geometry().top());
    }
  }
  g.nextcol = g.nextrow = 0;

  addCaption(g);
  
  for (QDomElement e=doc.firstChildElement(); !e.isNull();
       e=e.nextSiblingElement()) {
    QString tag = e.tagName();
    if (tag=="group")
      addButtonGroup(g,e);
    else if (tag=="button")
      addButton(g,e);
    else if (tag=="item")
      addItem(g,e);
    else if (tag=="tabbedpage")
      addTabbedPage(g,e);
    else if (tag=="page")
      addPage(g,e);
    else if (tag=="menu")
      addMenuPage(g,e);
    else if (tag=="checklist")
      addCheckListPage(g,e);
    else if (tag=="space")
      addSpace(g,e);
    else if (tag=="break") 
      addBreak(g,e);
    else
      fprintf(stderr,"Warning: xmlPage: Unexpected tag <%s>.\n",
	      qPrintable(tag));
  }
  hide();
}

xmlPage::~xmlPage() {
  for (QMap<QString, xmlButton *>::iterator i=buttons.begin();
       i!=buttons.end(); ++i)
    delete i.value();
  for (QMap<QString, xmlPage *>::iterator i=subPages.begin();
       i!=subPages.end(); ++i)
    delete i.value();
  // for (QMap<QString, QButtonGroup *>::iterator i=groups.begin();
  //      i!=groups.end(); ++i)
  //   delete i.value(); // not needed, our children auto-reaped by qt.
}

void xmlPage::addSpace(xmlPage::Geom &g, QDomElement doc) {
  bool ok;
  double dy = doc.hasAttribute("dy") ? doc.attribute("dy").toDouble(&ok) : 0.2;
  g.nextrow += dy;
}

void xmlPage::addBreak(xmlPage::Geom &g, QDomElement doc) {
  bool ok;
  double dx = doc.hasAttribute("dx") ? doc.attribute("dx").toDouble(&ok) : 0;
  g.nextrow=0;
  g.nextcol += 1+dx;
}

void xmlPage::addButtonGroup(xmlPage::Geom &g, QDomElement doc) {
  bool ok;
  QString id=xmlAttribute(doc,"id","xmlPage (addButtonGroup)",
			  "Cannot read id");
  int cols = doc.hasAttribute("cols") ? doc.attribute("cols").toInt(&ok) : 1;
  int col=0;
  QObject *bg = new QObject(this);
  //  QButtonGroup *bg = new QButtonGroup(this);
  groups[id] = bg;
  double startrow = g.nextrow;
  for (QDomElement e=doc.firstChildElement(); !e.isNull();
       e=e.nextSiblingElement()) {
    QString tag = e.tagName();
    if (tag=="button") {
      xmlButton *b = addButton(g,e);
      if (cols>1)
	b->resize(b->width()/cols-((col==cols-1)
				   ? 2
				   : (master->geom().intx*(cols-1))),
		  b->height());
      ButtonGrouper::add(b,bg);// bg->addButton(b);
      b->makeRadio();
      b->setVisualType(b->getID().contains("panel") ?
		       Button::VTPanelOpen : Button::VTGrouped);
      groupedButton[e.attribute("id")] = id;
    } else if (tag=="break") {
      g.nextcol += 1./cols;
      col++;
      g.nextrow=startrow;
    } else {
      fprintf(stderr,"Warning: xmlPage: Unexpected tag <%s> in group.\n",
	      qPrintable(tag));
    }
  }
  g.nextrow=startrow;
  g.nextcol += 1./cols;

  if (doc.hasAttribute("default"))
    groupDefaults[id] = doc.attribute("default"); 
}

xmlButton *xmlPage::addItem(xmlPage::Geom &g, QDomElement doc) {
  QObject *bg;
  if (groups.contains(":items")) {
    bg = groups[":items"];
  } else {
    bg = new QObject(this);
    groups[":items"] = bg;
  }
  QString id;
  if (doc.hasAttribute("id"))
    id=doc.attribute("id");
  else if (doc.hasAttribute("custom"))
    id="custom-"+doc.attribute("custom");
  else
    id=doc.attribute("value");
  if (id.isEmpty()) {
    id="--";
    fprintf(stderr,"Warning: xmlPage: empty button ID in %s. custom=%s.\n",
	    qPrintable(myPath),
	    qPrintable(doc.attribute("custom")));
  }
  
  xmlButton *b = new xmlButton(this,doc,pathToGlobal(id),master);
  buttons[id] = b;
  groupedButton[id] = ":items";
  
  if (g.rows>0 && g.nextrow>=g.rows) {
    g.nextrow=0; g.nextcol++;
  }

  b->setGeometry(int(g.button.x0+g.button.dx*g.nextcol),
		 int(g.button.y0+g.button.dy*g.nextrow),
		 g.button.w,
		 g.button.h);
  if (myTag=="checklist")
    b->makeToggle();
  else
    b->makeRadio();
  b->makeItem();
  b->setVisualType(Button::VTVarValue);
  g.nextrow++;
  return b;
}

xmlButton *xmlPage::addButton(xmlPage::Geom &g, QDomElement doc) {
  QString id=xmlAttribute(doc,"id",
			  "xmlPage (addButton)","Cannot read button ID");
  xmlButton *b = new xmlButton(this,doc,pathToGlobal(id),master);
  if (b->customNo()) {
    b->makeRadio();
    b->setVisualType(Button::VTVarValue);
  } else {
    b->makeAction();
    b->setVisualType(Button::VTAction);
  }
  
  buttons[id] = b;
  bool defaultconnections = true;

  if (doc.hasAttribute("type")) {
    QString t = doc.attribute("type");
    if (t=="bool") {
      connect(b,SIGNAL(selected(QString,QString)),
	      this,SLOT(booleanButtonToggled(QString)));
      connect(b,SIGNAL(deselected(QString,QString)),
	      this,SLOT(booleanButtonToggled(QString)));
      defaultconnections = false;
      QString txt = b->text();
      if (b->getSelected())
	txt.replace("Disabled","Enabled");
      else
	txt.replace("Enabled","Disabled");
      b->setText(txt);
      b->makeItem();
      b->setVisualType(Button::VTBooleanVar);
    } else {
      fprintf(stderr,"xmlPage(%s): warning: unknown type '%s' for button %s\n",
	      qPrintable(myPath),qPrintable(t),qPrintable(id));
    }
  }

  if (b->customNo()<0) {
    connect(b,SIGNAL(customize(QString,int,QString)),
	    this,SLOT(childItemCustomized(QString,int,QString)));
    defaultconnections=false;
  }

  if (defaultconnections) {
    connect(b,SIGNAL(selected(QString,QString)),
	    master,SIGNAL(buttonSelected(QString,QString)));
    connect(b,SIGNAL(deselected(QString,QString)),
	    master,SIGNAL(buttonDeselected(QString,QString)));
    connect(b,SIGNAL(activated(QString,QString)),
	    master,SIGNAL(buttonClicked(QString,QString)));
  }
  
  if (g.rows>0 && g.nextrow>=g.rows) {
    g.nextrow=0; g.nextcol++;
  }

  double col=g.nextcol;
  double row=g.nextrow;
  bool ok;
  if (doc.hasAttribute("x"))
    col=doc.attribute("x").toDouble(&ok);
  if (doc.hasAttribute("y"))
    row=doc.attribute("y").toDouble(&ok);
  g.nextrow=row+1;
  g.nextcol=col;
  b->setGeometry(int(g.button.x0+g.button.dx*col),
		 int(g.button.y0+g.button.dy*row),
		 g.button.w,
		 g.button.h);
  return b;
}

QString xmlPage::pathToLocal(QString path) const {
  QStringList list = path.split("/");
  QStringList us = myPath.split("/");
  QString elt = "";
  while (!us.isEmpty() && !myPath.isEmpty()) {
    QString ushead = us.takeFirst();
    QString themhead = list.takeFirst();
    // dbg("path='%s' mypath='%s' ushead='%s' themhead='%s'",
    //     qPrintable(path),qPrintable(myPath),
    //     qPrintable(ushead),qPrintable(themhead));
    int colonidx = themhead.indexOf(':');
    if (colonidx>=0) {
      if (us.isEmpty())
	elt = themhead.mid(colonidx);
      themhead = themhead.left(colonidx);
    }
    if (themhead!=ushead)
      throw Exception("xmlPage","'"+path+"' is not a descendent of '"
		      +myPath+"'","pathToLocal");
  }
  if (!elt.isEmpty())
    list.push_front(elt);
  return list.join("/");
}

QString xmlPage::pathToGlobal(QString leaf) const {
  QString path = myPath;
  if (!path.isEmpty() && !leaf.isEmpty() && leaf.left(1)!=":")
    path += "/";
  path+=leaf;
  return path;
}

QString xmlPage::pathDeinstantiate(QString path) const {
  if (path=="")
    return "";
  QStringList list = path.split("/");
  QStringList build;
  while (!list.isEmpty()) {
    QString bit = list.takeFirst();
    int colonidx = bit.indexOf(':');
    if (colonidx>=0)
      bit=bit.left(colonidx);
    build.push_back(bit);
  }
  return build.join("/");
}

QString xmlPage::pathInstantiate(QString path) const {
  if (path=="")
    return "";
  QStringList list = path.split("/");
  QStringList us = myPath.split("/");
  if (myPath.isEmpty())
    us=QStringList();
  if (list.size()<us.size())
    return path;
  if (myTag=="tabbedpage") {
    QString mybit = list[us.size()-1];
    int colonidx = mybit.indexOf(':');
    if (colonidx<0)
      list[us.size()-1] += ":" + currentElement;
  }
  if (list.size()>us.size()) {
    QString child = list[us.size()];
    int colonidx = child.indexOf(':');
    if (colonidx>=0)
      child=child.left(colonidx);
    if (subPages.contains(child))
      return subPages[child]->pathInstantiate(list.join("/"));
    else if (buttons.contains(child))
      return list.join("/");
    else
      throw Exception("xmlPage","'"+path+"' does not name a page or button","pathInstantiate");
  } else {
    return list.join("/");
  }
}

xmlPage *xmlPage::addPage(xmlPage::Geom &g, QDomElement doc,
			  Button::VisualType vt) {
  QString id=xmlAttribute(doc,"id",
			  "xmlPage (addPage)","Cannot read subpage ID");
  //fprintf(stderr,"xmlPage(%s)::addPage(%s)\n",
  //        qPrintable(myPath),qPrintable(id));
  ParamTree *subtree;
  if (myTag=="tabbedpage")
    subtree = ptree; /* I cannot instantiate tabbed pages' children's
			ptrees really here. See open(QString). */
  else
    subtree = &(ptree->child(id));
  xmlPage *p = new xmlPage(this, subtree,doc,master, pathToGlobal(id),
			   QRect(g.page.dxl,g.page.dyt,
				 width()-g.page.dxl-g.page.dxr,
				 height()-g.page.dyt-g.page.dyb));

  connect(p,SIGNAL(opening(QString,QWidget*)),
	  master,SIGNAL(pageOpening(QString,QWidget*)));
  connect(p,SIGNAL(opened(QString,QWidget*)),
	  master,SIGNAL(pageOpened(QString,QWidget*)));
  connect(p,SIGNAL(closed(QString,QWidget*)),
	  master,SIGNAL(pageClosed(QString,QWidget*)));

  subPages[id] = p;

  if (buttons.contains(id)) {
    xmlButton *b = buttons[id];
    connect(b,SIGNAL(selected(QString,QString)),p,SLOT(open()));
    connect(b,SIGNAL(deselected(QString,QString)),p,SLOT(close()));
    connect(b,SIGNAL(selected(QString,QString)),
	    this,SLOT(addTriangle(QString)));
    connect(b,SIGNAL(deselected(QString,QString)),
	    this,SLOT(removeTriangle(QString)));
    disconnect(b,SIGNAL(selected(QString,QString)),
	       master,SIGNAL(buttonSelected(QString,QString)));
    disconnect(b,SIGNAL(deselected(QString,QString)),
	       master,SIGNAL(buttonDeselected(QString,QString)));
    disconnect(b,SIGNAL(activated(QString,QString)),
	       master,SIGNAL(buttonClicked(QString,QString)));
    b->makeRadio(); // new 10/8/09
    b->setVisualType(vt);
    if (!doc.hasAttribute("bg")) {
      QPalette butpal = b->palette();
      QPalette pagepal = p->palette();
      QColor bg = butpal.color(QPalette::Normal,QPalette::Button);
      pagepal.setColor(QPalette::Window,bg);
      p->setPalette(pagepal);
      p->setAutoFillBackground(true);
      p->setFrameStyle(QFrame::Panel | QFrame::Raised);
    }
    QString t = b->text();
    if (t.indexOf(":")<0 && t.indexOf("...")<0)
      b->setText(t + " ...");
  }
  return p;
}

void xmlPage::addTriangle(QString id) {
  QStringList path = id.split("/"); id = path.last();
  triID = id;
  dbg("xmlPage::addTriangle. id=%s",qPrintable(id));
  xmlButton *b = buttons.contains(id) ? buttons[id] : 0;
  int idx = id.indexOf("*");
  if (idx>0)
    id=id.left(idx);
  xmlPage *p = subPages.contains(id) ? subPages[id] : 0;
  dbg("xmlPage::addTriangle. b=%p p=%p",b,p);
  if (b && p) {
    QRect br = b->geometry();
    QRect pr = p->geometry();
    int bx;
    int px;
    int bx0;
    if (br.left()+br.right() < pr.left() + pr.right()) {
      // button is to the left
      bx0 = br.right();
      bx = bx0 - 12;
      px = pr.left();
    } else {
      // button is to the right
      bx0 = br.left();
      bx = bx0 + 12;
      px = pr.right();
    }
    //    bx = (10*bx + br.left() + br.right()) / 12;
    triangle = QPolygon(4);
    triangle.setPoint(0,bx,br.top());
    triangle.setPoint(1,bx,br.bottom());
    triangle.setPoint(2,px,pr.bottom());
    triangle.setPoint(3,px,pr.top());
    if (!triangle.containsPoint(QPoint(bx0,br.top()),Qt::OddEvenFill))
      triangle.setPoint(0,bx0,br.top());
    if (!triangle.containsPoint(QPoint(bx0,br.bottom()),Qt::OddEvenFill))
      triangle.setPoint(1,bx0,br.bottom());
    QColor mid = b->palette().color(QPalette::Mid);
    QColor but = b->palette().color(QPalette::Button);
    if (b->getVisualType()==Button::VTArrayCtrl) 
      triColor = QColor(mid); //mid.red()/2+but.red()/2,
    //mid.green()/2+but.green()/2,
    //mid.blue()/2+but.blue()/2);
    else
      triColor = but;
  } else {
    triangle = QPolygon(0);
    triID = "";
  }
  update();
}

void xmlPage::removeTriangle(QString id) {
  QStringList path = id.split("/"); id = path.last();
  dbg("xmlPage::removeTriangle. id=%s",qPrintable(id));
  if (triID == id)
    triID = "";
  update();
}

void xmlPage::paintEvent(class QPaintEvent *e) {
  //  dbg("xmlPage::paintevent. triID=%s",qPrintable(triID));
  if (triID=="")
    return;
  QFrame::paintEvent(e);
  QPainter p(this);
  p.setBrush(triColor);
  p.setPen(QPen(Qt::NoPen));
  // for (int k=0; k<4; k++) {
  //   int x,y;
  //   triangle.point(k,&x,&y);
  //   dbg("xmlPage: polygon[%i] = (%i,%i)",k,x,y);
  // }
  p.drawPolygon(triangle);
}

xmlPage *xmlPage::addTabbedPage(xmlPage::Geom &g, QDomElement doc) {
  xmlPage *p = addPage(g,doc);
  QString id=doc.attribute("id");
  xmlButton *penable = p->buttons.contains("enable")
    ? p->buttons["enable"] : 0;
  dbg("addTabbedPage: penable=%p",penable);
  
  if (groups.contains(id)) {
    for (QMap<QString,QString>::iterator i=groupedButton.begin();
	 i!=groupedButton.end(); ++i) {
      if (i.value()==id) {
	xmlButton *b = buttons[i.key()];
	connect(b,SIGNAL(selected(QString,QString)),p,SLOT(open(QString)));
	disconnect(b,SIGNAL(selected(QString,QString)),
		   master,SIGNAL(buttonSelected(QString,QString)));
	disconnect(b,SIGNAL(deselected(QString,QString)),
		   master,SIGNAL(buttonDeselected(QString,QString)));
	disconnect(b,SIGNAL(activated(QString,QString)),
		   master,SIGNAL(buttonClicked(QString,QString)));
	connect(b,SIGNAL(selected(QString,QString)),
		this,SLOT(addTriangle(QString)));
	connect(b,SIGNAL(deselected(QString,QString)),
		this,SLOT(removeTriangle(QString)));
	b->setVisualType(Button::VTArrayCtrl);
	if (penable)
	  connect(b,SIGNAL(doubleClicked(QString,QString)),
		  penable,SLOT(toggleSelected()));
      }
    }
  }
  if (penable) {
    connect(p->buttons["enable"],SIGNAL(selected(QString,QString)),
	    this,SLOT(childTabEnabled(QString)));
    connect(p->buttons["enable"],SIGNAL(deselected(QString,QString)),
	    this,SLOT(childTabEnabled(QString)));
  }
  return p;
}

xmlPage *xmlPage::addMenuPage(xmlPage::Geom &g, QDomElement doc) {
  xmlPage *p = addPage(g,doc,Button::VTVarOpen);  
  for (QMap<QString,xmlButton *>::iterator i=p->buttons.begin();
       i!=p->buttons.end(); ++i) {
    xmlButton *b = i.value();
    if (b->getTag()=="item") {
      connect(b,SIGNAL(selected(QString,QString)),
	      this,SLOT(childItemSelected(QString,QString)));
      if (b->isCustom())
	connect(b,SIGNAL(customize(QString,int,QString)),
		this,SLOT(childItemCustomized(QString,int,QString)));
    }
  }
  return p;
}

xmlPage *xmlPage::addCheckListPage(xmlPage::Geom &g, QDomElement doc) {
  xmlPage *p = addPage(g,doc,Button::VTVarOpen);
  for (QMap<QString,xmlButton *>::iterator i=p->buttons.begin();
       i!=p->buttons.end(); ++i) {
    xmlButton *b = i.value();
    if (b->getTag()=="item") {
      connect(b,SIGNAL(selected(QString,QString)),
	      this,SLOT(childItemSelected(QString,QString)));
      connect(b,SIGNAL(deselected(QString,QString)),
	      this,SLOT(childItemDeselected(QString,QString)));
    }
  }
  return p;
}

void xmlPage::addCaption(xmlPage::Geom &g) {
  if (caption.isEmpty())
    return;
  QLabel *lbl = new QLabel(this);
  lbl->setText(caption);
  lbl->setGeometry(0,0,width(),g.caph);
  lbl->setFont(master->geom().buttonFont);
  lbl->setAlignment(Qt::AlignHCenter);
}

void xmlPage::open() {
  emit opening(pathInstantiate(myPath),(QWidget*)(this));
  Param *pp = ptree->leafp();
  //fprintf(stderr,"xmlPage(%s):open pp=%p\n",qPrintable(myPath),pp);
  if (pp) 
    openLeaf(pp);
  else
    openNode();
  if (neverOpened) {
    neverOpened = false;
    for (QMap<QString,QString>::iterator i=groupDefaults.begin();
	 i!=groupDefaults.end(); ++i) {
      QString butname = i.value();
      if (buttons.contains(butname)) {
	xmlButton *b = buttons[butname];
	b->setSelected(true);
      }
    }
  }
  for (QMap<QString,xmlPage *>::iterator i=subPages.begin();
       i!=subPages.end(); ++i) {
    if (buttons.contains(i.key())) {
      xmlButton *b = buttons[i.key()];
      if (b->getSelected())
	i.value()->open();
    }
  }

  show();
  emit opened(pathInstantiate(myPath),(QWidget*)(this));
}

void xmlPage::open(QString p) {
  int idx = p.lastIndexOf('/');
  QString elt = p.mid(idx+1);
  idx = elt.indexOf('*');
  QString ar = (idx>=0) ? elt.left(idx) : "";
  elt = elt.mid(idx+1);
  currentElement = elt;

  //fprintf(stderr,"xmlPage(%s):open(%s) ar='%s' elt='%s'\n",
  //        qPrintable(myPath),qPrintable(p),
  //        qPrintable(ar),qPrintable(elt));

  reTree(0);
  open();
}

void xmlPage::reTree(ParamTree *neworigtree) {
  if (neworigtree)
    origptree = neworigtree;

  ptree = origptree->childp(currentElement);
  if (!ptree)
    ptree = origptree;

  for (QMap<QString, xmlPage *>::iterator i=subPages.begin();
       i!=subPages.end(); ++i) 
    i.value()->reTree(ptree->childp(i.key()));

  if (isVisible() && neworigtree!=0)
    open();
}

void xmlPage::openLeaf(Param *pp) {
  if (pp->getType()=="set") {
    QBitArray ba = pp->toBitArray();
    for (QMap<QString, class xmlButton *>::iterator i=buttons.begin();
	 i!=buttons.end(); ++i) {
      xmlButton *b = i.value();
      if (b->getTag()=="item") {
	Enumerator const *e=pp->getEnum();
	try {
	  int idx=e->lookup(b->getValue());
	  b->setSelected(ba.testBit(idx));
	} catch (Exception) {
	  fprintf(stderr,"Ignored.\n");
	}	  
      }
    }
  } else {
    Param copy(*pp);
    bool valuefound = false;
    for (QMap<QString, class xmlButton *>::iterator i=buttons.begin();
	 i!=buttons.end(); ++i) {
      xmlButton *b = i.value();

      if (b->isCustom()) {
	Param const *custpar = master->findpParam(QString("custom/%1-%3")
						  .arg(pathDeinstantiate(myPath))
						  .arg(b->customNo()));
	if (custpar)
	  b->setValue(custpar->toString());
	else
	  fprintf(stderr,"xmlPage(%s): Warning: no value for custom %i\n",
		  qPrintable(myPath), b->customNo());
      }
      try {
	copy.set(b->getValue());
	bool match = copy==*pp;
	b->setSelected(match);
	if (match)
	  valuefound=true;
	//fprintf(stderr,"xmlPage(%s):openleaf: parpath=%s.\n",
	//        qPrintable(myPath),qPrintable(master->pathInstantiate(myPath)));
	b->setEnabled(master->canSetParam(master->pathInstantiate(myPath),b->getValue()));
      } catch (Exception) {
	if (b->getTag()=="item")
	  b->setEnabled(false);
      }
    }
    if (!valuefound) {
      // try to locate a custom button
      for (QMap<QString, class xmlButton *>::iterator i=buttons.begin();
	   i!=buttons.end(); ++i) {
	xmlButton *b = i.value();
	if (b->isCustom()) {
	  b->setValue(pp->toString());
	  b->setSelected(true);
	  break;
	}
      }
    }
  }
}

void xmlPage::openNode() {
  bool enable = true;
  Param *p = ptree->findp("enable");
  if (p)
    enable = p->toBool();
  setEnabled(enable,"enable");

  for (QMap<QString, class xmlButton *>::iterator i=buttons.begin();
       i!=buttons.end(); ++i) {
    xmlButton *b = i.value();
    Param *p = ptree->findp(i.key());
    if (p) {
      // This is a button that directly represents a value.
      bool ena = p->isEnabled();
      if (!ena) {
	b->setEnabled(false);
	p->restore();
	if (subPages.contains(i.key())) {
	  subPages[i.key()]->hide();
	  b->setSelected(false);
	}
      }
      if (p->getType()=="bool") {
	b->makeToggle();
	b->setSelected(p->toBool());
      } else {
	b->setValue(p->toString());
      }
    } else if (i.key().indexOf('*')>0) {
      // This is a button that represents a tab.
      representTabEnabled(i.key());
    }
  }

}

void xmlPage::representTabEnabled(QString id) {
  xmlButton *b = buttons[id];
  int idx = id.indexOf('*');
  QString ar = id.left(idx);
  QString elt = id.mid(idx+1);
  Param *p = origptree->findp(ar+"/"+elt+"/enable");
  if (p) {
    QPalette pa = b->palette();
    pa.setColor(QPalette::Mid,
		p->toBool() ? QColor("#ff2200") : QColor("#884444"));
    b->setPalette(pa);
  }
  Dbg() << "representTabEnabled: " << id << " " << triID;
  if (id==triID)
    addTriangle(id);
}

void xmlPage::close() {
  hide();
  for (QMap<QString,xmlPage *>::iterator i=subPages.begin();
       i!=subPages.end(); ++i) {
    if (buttons.contains(i.key())) {
      xmlButton *b = buttons[i.key()];
      if (b->getSelected())
	i.value()->close();
    }
  }
  emit closed(pathInstantiate(myPath),(QWidget*)this);
}

void xmlPage::childTabEnabled(QString path) {
  QString child = pathToLocal(path);
  child = child.left(child.indexOf('/'));
  //fprintf(stderr,"xmlPage(%s)::childTabEnabled(%s) child=%s\n",
  //        qPrintable(myPath),
  //        qPrintable(path),
  //        qPrintable(child));
  if (subPages.contains(child)) {
    QString but = child + "*" + subPages[child]->currentElement;
    //fprintf(stderr,"  childTabEnabled but='%s'\n",qPrintable(but));
    representTabEnabled(but);
  }
}

xmlPage *xmlPage::findpPage(QStringList path) {
  if (path.isEmpty())
    return this;
  QString head = path.takeFirst();
  if (subPages.contains(head))
    return subPages[head]->findpPage(path);
  else
    return 0;
}

xmlPage &xmlPage::findPage(QStringList path) {
  xmlPage *pg = findpPage(path);
  if (pg)
    return *pg;
  else
    throw Exception("xmlPage", "Cannot find page '" + path.join("/") + "'");
}

xmlButton *xmlPage::findpButton(QStringList path) {
  QString head = path.takeFirst();
  if (path.isEmpty()) {
    if (buttons.contains(head))
      return buttons[head];
    else
      return 0;
  } else {
    if (subPages.contains(head))
      return subPages[head]->findpButton(path);
    else
      return 0;
  }
}

xmlButton &xmlPage::findButton(QStringList path) {
  xmlButton *b = findpButton(path);
  if (b)
    return *b;
  else
    throw Exception("xmlPage", "Cannot find button '" + path.join("/") +"'");
}

void reportqbit(QBitArray const &ba) {
  fprintf(stderr,"n=%i [",ba.size());
  for (int i=0; i<ba.size(); i++)
    fprintf(stderr,"%i",ba.at(i));
  fprintf(stderr,"].\n");
}

void xmlPage::booleanButtonToggled(QString path) {
  // dbg("xmlPage(%s)::booleanButtonToggled(%s)\n",
  //         qPrintable(myPath),
  //         qPrintable(path));
  QString parpath = master->pathInstantiate(path);
  QString local = pathToLocal(path);
  xmlButton *b = buttons[local];
  bool on = b->getSelected();
  master->setParam(parpath,on ? "yes" : "no");
  if (local=="enable")
    setEnabled(on,local);
  else
    updateEnabled();
  QString txt = b->text();
  if (on) 
    txt.replace("Disabled","Enabled");
  else
    txt.replace("Enabled","Disabled");
  b->setText(txt);
}

void xmlPage::updateEnabled() {
  xmlButton *enabutp = findpButton(QString("enable").split('/'));
  bool pageEna = enabutp ? enabutp->getSelected() : true;
  for (QMap<QString, xmlButton *>::iterator i=buttons.begin();
       i!=buttons.end(); ++i) {
    Param *p = ptree->findp(i.key());
    if (p) {
      bool ena = p->isEnabled();
      i.value()->setEnabled(ena &&
			    (pageEna
			     || i.key().startsWith("_")
			     || i.key()=="enable"));
      if (!ena) {
	//	p->restore();
	i.value()->setValue(p->toString());
      }
    }
  }
}  

void xmlPage::setEnabled(bool enable, QString enabler) {
  // dbg("xmlPage::setenabled(%i,%s)",enable,qPrintable(enabler));
  for (QMap<QString, xmlButton *>::iterator i=buttons.begin();
       i!=buttons.end(); ++i) {
    // Dbg() << "  key: " << i.key();
    if (i.key()!=enabler) {
      if (enable || i.key().startsWith("@") || i.key().startsWith("_@")) {
	Param *p = ptree->findp(i.key());
	bool ena = p ? p->isEnabled() : true;
	// Dbg() << "  value: " << i.value() << " ena=" << ena;
	i.value()->setEnabled(ena);
	//if (!ena)
	  //p->restore();
      } else {
	i.value()->setEnabled(false);
      }
    }
  }
  for (QMap<QString, xmlPage *>::iterator i=subPages.begin();
       i!=subPages.end(); ++i)
    if (i.value()->isVisible())
      i.value()->setEnabled(enable,"");

  if (enable && isVisible() && ptree->leafp()!=0)
    openLeaf(ptree->leafp());
}

void xmlPage::childItemSelected(QString path, QString) {
  // fprintf(stderr,"xmlPage(%s)::childItemSelected(%s,%s)\n",
  //         qPrintable(myPath),
  //         qPrintable(path),
  //         qPrintable(text));
  QString parpath = master->pathInstantiate(path);
  QStringList plist = parpath.split('/');
  QString v = plist.takeLast(); // That's the value, not the realpath
  parpath = plist.join("/");

  QString local=pathToLocal(path);
  QString child = local.left(local.indexOf('/'));

  xmlButton *b=findpButton(local.split('/'));
  QString value = b?b->getValue() : "0";
  //   fprintf(stderr,"  parpath=%s. value=%s. local=%s. child=%s. b=%p v=%s\n",
  //           qPrintable(parpath),
  //           qPrintable(value),
  //           qPrintable(local),
  //           qPrintable(child),
  // 	  b,
  // 	  qPrintable(v));
  if (b) {
    if (b->isCustom()) {
      if (master->canSetParam(parpath,value)) {
	master->setParam(parpath,value);
	buttons[child]->setValue(value); // this already happened???
      } else {
	fprintf(stderr,"INVALID CUSTOM VALUE\n");
      }
    } else if (subPages[child]->myTag=="checklist") {
      master->setParam(parpath,"+"+value);
    } else {
      master->setParam(parpath,value);
      buttons[child]->setValue(value);
    }
  }
  //ptree->find(pathToLocal(parpath)).report();
  updateEnabled();
}

void xmlPage::childItemDeselected(QString path, QString) {
  //fprintf(stderr,"xmlPage(%s)::childItemDeselected(%s,%s)\n",
  //        qPrintable(myPath),
  //        qPrintable(path),
  //        qPrintable(text));
  QString parpath = master->pathInstantiate(path);
  QStringList plist = parpath.split('/');
  QString value = plist.takeLast(); // That's the value, not the realpath
  parpath = plist.join("/");

  QString local=pathToLocal(path);
  QString child = local.left(local.indexOf('/'));

  xmlButton *b=findpButton(local.split('/'));
  if (b) {
    if (subPages[child]->myTag=="checklist") {
      master->setParam(parpath,"-"+value);
      //ptree->find(pathToLocal(parpath)).report();
    }
  }
}

void xmlPage::childItemCustomized(QString path, int cno, QString text) {
  fprintf(stderr,"xmlPage(%s)::childItemCustomized(%s,%i,%s)\n",
          qPrintable(myPath),
          qPrintable(path),
          cno,
          qPrintable(text));
  QString parampath = master->pathInstantiate(path);
  if (cno>0)
    parampath = parampath.left(parampath.lastIndexOf('/'));
  if (master->canSetParam(parampath,text)) {
    master->setParam(parampath,text);
    if (cno>0)
      master->setCustom(pathDeinstantiate(parampath),cno,text);
    xmlButton *b = findpButton(pathToLocal(path).split('/'));
    if (!b)
      throw Exception("xmlPage","Cannot find custom button","(childItemCustomized)");
    QString v = master->findParam(parampath).toString();
    b->setValue(v);
    if (cno>0)
      b->setSelected(true);
    b->closeEditor();
    if (cno>0)
      childItemSelected(path,v);
  } else {
    QString child = pathToLocal(path);
    child = child.left(child.indexOf('/'));
    QMessageBox::warning(this, child,
			 tr("Invalid value. Please modify or Cancel."),
			 QMessageBox::Ok);
  }
}

QList<xmlButton *> xmlPage::getGroup(QString id) const {
  QList<xmlButton *> list;
  if (!groups.contains(id))
    return list;
  for (QMap<QString,QString>::const_iterator i=groupedButton.begin();
       i!=groupedButton.end(); ++i)
    if (i.value()==id)
      list.push_back(buttons[i.key()]);
  return list;
}
