// guipage.cpp

#include <gui/guipage.h>

#include <QWidget>
#include <QButtonGroup>
#include <xml/paramtree.h>
#include <xml/attribute.h>
#include <gui/guigeom.h>
#include <gui/guiroot.h>
#include <xml/enumerator.h>
#include <gui/guibutton.h>
#include <QRect>
#include <QMessageBox>
#include <QLabel>
#include <base/exception.h>
#include <base/dbg.h>
#include <gfx/buttongrouper.h>
#include <QPainter>
#include <QPen>
#include <gui/guibuttongroup.h>

guiPage::guiPage(class QWidget *parent,
		 class ParamTree *ptree_,
		 QDomElement doc,
		 class guiRoot *master_,
		 QString mypath,
		 class QRect const &geom):
  AbstractPage(parent, ptree_, doc, master_, mypath, geom),
  buildGeom(this), autoInfo(this) {

  setLineWidth(0);
  neverOpened=true;
  autoInfo.hasAutoItems = false;

  currentElement = "";

  triID = "";

  setDefaultColors(doc);

  buildGeom.setup(doc);
  addChildren(buildGeom, doc);

  hide();
}

guiPage::~guiPage() {
  foreach (guiButton *b, buttons)
    delete b;
}

void guiPage::addSpace(PageBuildGeom &g, QDomElement doc) {
  bool ok;
  double dy = doc.hasAttribute("dy") ? doc.attribute("dy").toDouble(&ok) : 0.2;
  g.nextrow += dy;
}

void guiPage::addBreak(PageBuildGeom &g, QDomElement doc) {
  bool ok;
  double dx = doc.hasAttribute("dx") ? doc.attribute("dx").toDouble(&ok) : 0;
  g.nextrow = 0;
  g.nextcol += 1+dx;
}

void guiPage::addButtonGroup(PageBuildGeom &g, QDomElement doc) {
  guiButtonGroup *bg = new guiButtonGroup(this);
  bg->build(g, doc);
  groups[bg->groupId()] = bg;
  foreach (QString s, bg->childIDs())
    groupedButton[s] = bg->groupId();
}

guiButton *guiPage::addItem(PageBuildGeom &g, QDomElement doc) {
  guiButtonGroup *bg;
  if (groups.contains(":items")) {
    bg = groups[":items"];
  } else {
    bg = new guiButtonGroup(this);
    /* I will probably want an ItemGroup */
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
    fprintf(stderr,"Warning: guiPage: empty button ID in %s. custom=%s.\n",
	    qPrintable(myPath),
	    qPrintable(doc.attribute("custom")));
  }
  
  guiButton *b = new guiButton(this,doc,pathToGlobal(id),master);
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

guiButton *guiPage::addButton(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addButton)","Cannot read button ID");
  guiButton *b = new guiButton(this,doc,pathToGlobal(id),master);
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
      fprintf(stderr,"guiPage(%s): warning: unknown type '%s' for button %s\n",
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
    //    Dbg() << "Connecting doubleclicked" << id;
    connect(b,SIGNAL(doubleClicked(QString,QString)),
	    master,SIGNAL(buttonDoubleClicked(QString,QString)));
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


guiPage *guiPage::addPage(PageBuildGeom &g, QDomElement doc,
			  Button::VisualType vt) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addPage)","Cannot read subpage ID");
  //fprintf(stderr,"guiPage(%s)::addPage(%s)\n",
  //        qPrintable(myPath),qPrintable(id));
  ParamTree *subtree;
  if (myTag=="tabbedpage")
    subtree = ptree; /* I cannot instantiate tabbed pages' children's
			ptrees really here. See open(QString). */
  else
    subtree = &(ptree->child(id));
  guiPage *p = new guiPage(this, subtree,doc,master, pathToGlobal(id),
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
    guiButton *b = buttons[id];
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
      b->setText(t + "&nbsp;...");
  }
  return p;
}

void guiPage::addTriangle(QString id) {
  QStringList path = id.split("/"); id = path.last();
  triID = id;
  //  dbg("guiPage::addTriangle. id=%s",qPrintable(id));
  guiButton *b = buttons.contains(id) ? buttons[id] : 0;
  int idx = id.indexOf("*");
  if (idx>0)
    id=id.left(idx);
  guiPage *p = subPages.contains(id) ? dynamic_cast<guiPage*>(subPages[id]) : 0;
  //  dbg("guiPage::addTriangle. b=%p p=%p",b,p);
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

void guiPage::removeTriangle(QString id) {
  QStringList path = id.split("/"); id = path.last();
  //dbg("guiPage::removeTriangle. id=%s was=%s", qPrintable(id), qPrintable(triID));
  if (triID == id)
    triID = "";
  update();
}

void guiPage::paintEvent(class QPaintEvent *e) {
  //  dbg("guiPage::paintevent. triID=%s",qPrintable(triID));
  if (triID=="")
    return;
  QPainter p(this);
  p.setBrush(triColor);
  p.setPen(QPen(Qt::NoPen));
  // for (int k=0; k<4; k++) {
  //   int x,y;
  //   triangle.point(k,&x,&y);
  //   dbg("guiPage: polygon[%i] = (%i,%i)",k,x,y);
  // }
  p.drawPolygon(triangle);
  QFrame::paintEvent(e);
}

guiPage *guiPage::addTabbedPage(PageBuildGeom &g, QDomElement doc) {
  guiPage *p = addPage(g,doc);
  QString id=doc.attribute("id");
  guiButton *penable = p->buttons.contains("enable")
    ? p->buttons["enable"] : 0;
  //  dbg("addTabbedPage: penable=%p",penable);
  
  if (groups.contains(id)) {
    for (QMap<QString,QString>::iterator i=groupedButton.begin();
	 i!=groupedButton.end(); ++i) {
      if (i.value()==id) {
	guiButton *b = buttons[i.key()];
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
	//Dbg() << "Connecting doubleclicked" << id;
	connect(b,SIGNAL(doubleClicked(QString,QString)),
		master,SIGNAL(buttonDoubleClicked(QString,QString)));
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

guiPage *guiPage::addMenuPage(PageBuildGeom &g, QDomElement doc) {
  guiPage *p = addPage(g,doc,Button::VTVarOpen);  
  for (QMap<QString,guiButton *>::iterator i=p->buttons.begin();
       i!=p->buttons.end(); ++i) {
    guiButton *b = i.value();
    if (b->getTag()=="item") {
      connect(b,SIGNAL(selected(QString,QString)),
	      this,SLOT(childItemSelected(QString,QString)));
      if (b->isCustom())
	connect(b,SIGNAL(customize(QString,int,QString)),
		this,SLOT(childItemCustomized(QString,int,QString)));
    }
  }
  QString id=xmlAttribute(doc,"id");
  if (buttons.contains(id)) 
    buttons[id]->ensureValueInLabel();

  return p;
}

guiPage *guiPage::addCheckListPage(PageBuildGeom &g, QDomElement doc) {
  guiPage *p = addPage(g,doc,Button::VTVarOpen);
  for (QMap<QString,guiButton *>::iterator i=p->buttons.begin();
       i!=p->buttons.end(); ++i) {
    guiButton *b = i.value();
    if (b->getTag()=="item") {
      connect(b,SIGNAL(selected(QString,QString)),
	      this,SLOT(childItemSelected(QString,QString)));
      connect(b,SIGNAL(deselected(QString,QString)),
	      this,SLOT(childItemDeselected(QString,QString)));
    }
  }
  return p;
}

void guiPage::open() {
  emit opening(pathInstantiate(myPath),(QWidget*)(this));
  Param *pp = ptree->leafp();
  //fprintf(stderr,"guiPage(%s):open pp=%p\n",qPrintable(myPath),pp);
  if (pp) 
    openLeaf(pp);
  else
    openNode();
  if (neverOpened) {
    neverOpened = false;
    foreach (guiButtonGroup *bg, groups) 
      bg->selectDefaultButton();
    buildAutoItems();
  }
  foreach (QString id, subPages.keys()) {
    if (buttons.contains(id)) {
      guiButton *b = buttons[id];
      if (b->getSelected())
	subPages[id]->open();
    }
  }

  show();
  emit opened(pathInstantiate(myPath),(QWidget*)(this));
}

void guiPage::open(QString p) {
  int idx = p.lastIndexOf('/');
  QString elt = p.mid(idx+1);
  idx = elt.indexOf('*');
  QString ar = (idx>=0) ? elt.left(idx) : "";
  elt = elt.mid(idx+1);
  currentElement = elt;

  reTree(0);
  open();
}

void guiPage::openLeaf(Param *pp) {
  if (pp->getType()=="set") {
    QBitArray ba = pp->toBitArray();
    for (QMap<QString, class guiButton *>::iterator i=buttons.begin();
	 i!=buttons.end(); ++i) {
      guiButton *b = i.value();
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
    for (QMap<QString, class guiButton *>::iterator i=buttons.begin();
	 i!=buttons.end(); ++i) {
      guiButton *b = i.value();

      if (b->isCustom()) {
	Param const *custpar = master->paramTree().
	  findp(QString("custom/%1-%3")
		.arg(pathDeinstantiate(myPath))
		.arg(b->customNo()));
	if (custpar) {
	  b->setValue(custpar->toString());
	} else {
	  fprintf(stderr,"guiPage(%s): Warning: no value for custom %i\n",
		  qPrintable(myPath), b->customNo());
	}
      }
      try {
	copy.set(b->getValue());
	bool match = copy==*pp;
	b->setSelected(match);
	if (match)
	  valuefound=true;
	//fprintf(stderr,"guiPage(%s):openleaf: parpath=%s.\n",
	//        qPrintable(myPath),qPrintable(master->pathInstantiate(myPath)));
	b->setEnabled(master->canSetParam(master->pathInstantiate(myPath),b->getValue()));
      } catch (Exception) {
	if (b->getTag()=="item")
	  b->setEnabled(false);
      }
    }
    if (!valuefound) {
      // try to locate a custom button
      for (QMap<QString, class guiButton *>::iterator i=buttons.begin();
	   i!=buttons.end(); ++i) {
	guiButton *b = i.value();
	if (b->isCustom()) {
	  b->setValue(pp->toString());
	  b->setSelected(true);
	  break;
	}
      }
    }
  }
}

void guiPage::openNode() {
  bool enable = true;
  Param *p = ptree->findp("enable");
  if (p)
    enable = p->toBool();
  setEnabled(enable,"enable");

  for (QMap<QString, class guiButton *>::iterator i=buttons.begin();
       i!=buttons.end(); ++i) {
    guiButton *b = i.value();
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

void guiPage::representTabEnabled(QString id) {
  guiButton *b = buttons[id];
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
  //  Dbg() << "representTabEnabled: " << id << " " << triID;
  if (id==triID)
    addTriangle(id);
}

void guiPage::close() {
  hide();
  foreach (QString id, subPages.keys()) {
    if (buttons.contains(id)) {
      guiButton *b = buttons[id];
      if (b->getSelected())
	subPages[id]->close();
    }
  }
  emit closed(pathInstantiate(myPath),(QWidget*)this);
}

void guiPage::childTabEnabled(QString path) {
  QString child = pathToLocal(path);
  child = child.left(child.indexOf('/'));
  //fprintf(stderr,"guiPage(%s)::childTabEnabled(%s) child=%s\n",
  //        qPrintable(myPath),
  //        qPrintable(path),
  //        qPrintable(child));
  if (subPages.contains(child)) {
    QString but = child + "*" + subPages[child]->getCurrentElement();
    //fprintf(stderr,"  childTabEnabled but='%s'\n",qPrintable(but));
    representTabEnabled(but);
  }
}

void reportqbit(QBitArray const &ba) {
  fprintf(stderr,"n=%i [",ba.size());
  for (int i=0; i<ba.size(); i++)
    fprintf(stderr,"%i",ba.at(i));
  fprintf(stderr,"].\n");
}

void guiPage::booleanButtonToggled(QString path) {
  // dbg("guiPage(%s)::booleanButtonToggled(%s)\n",
  //         qPrintable(myPath),
  //         qPrintable(path));
  QString parpath = master->pathInstantiate(path);
  QString local = pathToLocal(path);
  guiButton *b = buttons[local];
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

void guiPage::updateEnabled() {
  guiButton *enabutp = findpButton(QString("enable").split('/'));
  bool pageEna = enabutp ? enabutp->getSelected() : true;
  for (QMap<QString, guiButton *>::iterator i=buttons.begin();
       i!=buttons.end(); ++i) {
    Param *p = ptree->findp(i.key());
    if (p) {
      bool ena = p->isEnabled();
      i.value()->setEnabled(ena &&
			    (pageEna
			     || i.key().startsWith("@")
			     || i.key()=="enable"));
      if (!ena) {
	//	p->restore();
	i.value()->setValue(p->toString());
      }
    }
  }
}  

void guiPage::setEnabled(bool enable, QString enabler) {
  // dbg("guiPage::setenabled(%i,%s)",enable,qPrintable(enabler));
  for (QMap<QString, guiButton *>::iterator i=buttons.begin();
       i!=buttons.end(); ++i) {
    // Dbg() << "  key: " << i.key();
    if (i.key()!=enabler) {
      if (enable || i.key().startsWith("@")) {
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
  foreach (AbstractPage *p0, subPages) {
    guiPage *p = dynamic_cast<guiPage *>(p0);
    if (p->isVisible())
      p->setEnabled(enable,"");
  }
  
  if (enable && isVisible() && ptree->leafp()!=0)
    openLeaf(ptree->leafp());
}

void guiPage::childItemSelected(QString path, QString) {
  QString parpath = master->pathInstantiate(path);
  QStringList plist = parpath.split('/');
  QString v = plist.takeLast(); // That's the value, not the realpath
  parpath = plist.join("/");

  QString local=pathToLocal(path);
  QString child = local.left(local.indexOf('/'));

  guiButton *b=findpButton(local.split('/'));
  QString value = b?b->getValue() : "0";
  if (b) {
    if (b->isCustom()) {
      if (master->canSetParam(parpath,value)) {
	master->setParam(parpath,value);
	buttons[child]->setValue(value);
      } else {
	fprintf(stderr,"INVALID CUSTOM VALUE\n");
      }
    } else if (subPages[child]->myTag=="checklist") {
      master->setParam(parpath, "+" + value);
    } else {
      master->setParam(parpath,value);
      buttons[child]->setValue(value);
    }
  }
  //ptree->find(pathToLocal(parpath)).report();
  updateEnabled();
}

void guiPage::childItemDeselected(QString path, QString) {
  //fprintf(stderr,"guiPage(%s)::childItemDeselected(%s,%s)\n",
  //        qPrintable(myPath),
  //        qPrintable(path),
  //        qPrintable(text));
  QString parpath = master->pathInstantiate(path);
  QStringList plist = parpath.split('/');
  QString value = plist.takeLast(); // That's the value, not the realpath
  parpath = plist.join("/");

  QString local=pathToLocal(path);
  QString child = local.left(local.indexOf('/'));

  guiButton *b=findpButton(local.split('/'));
  if (b) {
    if (subPages[child]->myTag=="checklist") {
      master->setParam(parpath,"-"+value);
      //ptree->find(pathToLocal(parpath)).report();
    }
  }
}

void guiPage::childItemCustomized(QString path, int cno, QString text) {
  fprintf(stderr,"guiPage(%s)::childItemCustomized(%s,%i,%s)\n",
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
    guiButton *b = findpButton(pathToLocal(path).split('/'));
    if (!b)
      throw Exception("guiPage","Cannot find custom button",
		      "(childItemCustomized)");
    QString v = master->paramTree().find(parampath).toString();
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

QList<guiButton *> guiPage::getGroup(QString id) const {
  QList<guiButton *> list;
  if (!groups.contains(id))
    return list;
  for (QMap<QString,QString>::const_iterator i=groupedButton.begin();
       i!=groupedButton.end(); ++i)
    if (i.value()==id)
      list.push_back(buttons[i.key()]);
  return list;
}


void guiPage::addAuto(PageBuildGeom &g, QDomElement doc) {
  autoInfo.setup(g, doc);
}

void guiPage::buildAutoItems() {
  if (!autoInfo.hasAutoItems)
    return;
  Dbg() << "guiPage::buildAutoItems " << myPath;
  // PageBuildGeom g = autoInfo.initialGeom;
  if (!ptree->leafp())
    throw Exception("guiPage", "Cannot have <auto> outside of a menu");
  Enumerator const *e = ptree->leafp()->getEnum();
  Dbg() << "  bai: enum="<<e;
  Dbg() << "  bai: items = " << e->getAllTags().join(" ");
}

void guiPage::setDefaultColors(QDomElement doc) {
  /* Set color and default button color */
  QString bg = doc.attribute("bg");
  if (!bg.isEmpty()) {
    QPalette p=palette();
    p.setColor(QPalette::Window, QColor(bg));
    setPalette(p);
    setAutoFillBackground(true);
    setFrameStyle(QFrame::Panel | QFrame::Raised);
  }
  if (doc.hasAttribute("bbg")) {
    QString bg = doc.attribute("bbg");
    QPalette p=palette();
    p.setColor(QPalette::Button, QColor(bg));
    setPalette(p);
  }    
}

void guiPage::addChildren(PageBuildGeom g, QDomElement doc) {
  /* Build children */
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
    else if (tag=="auto")
      addAuto(g, e);
    else
      fprintf(stderr,"Warning: guiPage: Unexpected tag <%s>.\n",
	      qPrintable(tag));
  }
}

QString guiPage::getCurrentElement() const {
  return currentElement;
}

guiButton const *guiPage::buttonp(QString id) const {
  if (buttons.contains(id))
    return buttons[id];
  else
    return 0;
}

guiButton *guiPage::buttonp(QString id) {
  if (buttons.contains(id))
    return buttons[id];
  else
    return 0;
}

guiButton &guiPage::button(QString id) {
  guiButton *b = buttonp(id);
  if (b)
    return *b;
  else
    throw Exception("guiPage",
		    "No button named '" + id + "' in page '" + myPath + "'");
}

guiPage *guiPage::findpPage(QStringList path) {
  AbstractPage *p = AbstractPage::findpPage(path);
  return dynamic_cast<guiPage *>(p);
}

guiPage &guiPage::findPage(QStringList path) {
  AbstractPage &p = AbstractPage::findPage(path);
  return dynamic_cast<guiPage &>(p);
}
