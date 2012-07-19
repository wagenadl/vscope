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
#include <gui/guiitem.h>
#include <QRect>
#include <QMessageBox>
#include <QLabel>
#include <base/exception.h>
#include <base/dbg.h>
#include <gfx/buttongrouper.h>
#include <QPainter>
#include <QPen>
#include <gui/guibuttongroup.h>
#include <gui/guimenu.h>
#include <gui/guichecklist.h>
#include <gui/guitabbedpage.h>
#include <gui/guitriangle.h>

#include <base/istype.h>

guiPage::guiPage(class QWidget *parent,
		 class ParamTree *ptree_,
		 QString id,
		 class guiRoot *master_,
		 class QRect const &geom):
  AbstractPage(parent, ptree_, id, master_, geom),
  buildGeom(this), autoInfo(this) {

  triangle = new guiTriangle(this);

  setLineWidth(0);
  neverOpened=true;
  autoInfo.hasAutoItems = false;

  currentElement = "";
  hide();
}

void guiPage::setup(QDomElement doc) {
  setDefaultColors(doc);
  buildGeom.setup(doc);
  addChildren(buildGeom, doc);
  connectToMaster(doc);
  connectToParent(doc);
  origGeom = geometry();
  sizeToFit();
}

void guiPage::sizeToFit() {
  QRect b0 = QRect(0, 0, origGeom.width(), origGeom.height());
  QRect bb = buildGeom.boundingBox();
  Dbg() << "guiPage["<<path()<<"]: sizetofit b0="<<b0<<"; bb="<<bb;
  bb.setWidth(bb.width()+bb.left());
  bb.setHeight(bb.height()+bb.top());
  bb &= b0;
  int dx = width() - (bb.right()+1);
  int dy = height() - (bb.bottom()+1);
  resize(bb.right()+1, bb.bottom()+1);
  move(origGeom.left() + dx/3, origGeom.top() + dy/3);
}

void guiPage::connectToMaster(QDomElement) {
  connect(this, SIGNAL(opening(QString,QWidget*)),
	  master, SIGNAL(pageOpening(QString,QWidget*)));
  connect(this, SIGNAL(opened(QString,QWidget*)),
	  master, SIGNAL(pageOpened(QString,QWidget*)));
  connect(this, SIGNAL(closed(QString,QWidget*)),
	  master, SIGNAL(pageClosed(QString,QWidget*)));
}

void guiPage::connectToParent(QDomElement doc) {
  guiPage *par = dynamic_cast<guiPage *>(parent());
  if (!par)
    return;
  
  guiButton *b = par->buttonp(id());
  if (b) {
    connect(b, SIGNAL(selected(QString,QString)),this, SLOT(open()));
    connect(b, SIGNAL(deselected(QString,QString)),this, SLOT(close()));
    connect(b, SIGNAL(selected(QString,QString)),
	    par, SLOT(addTriangle(QString)));
    connect(b,SIGNAL(deselected(QString,QString)),
	    par, SLOT(removeTriangle(QString)));
    disconnect(b, SIGNAL(selected(QString,QString)),
	       master, SIGNAL(buttonSelected(QString,QString)));
    disconnect(b, SIGNAL(deselected(QString,QString)),
	       master, SIGNAL(buttonDeselected(QString,QString)));
    disconnect(b, SIGNAL(activated(QString,QString)),
	       master, SIGNAL(buttonClicked(QString,QString)));

    b->makeRadio();
    b->setVisualType(visualTypeForParentButton());

    if (!doc.hasAttribute("bg")) {
      // Take background color from parent button
      QPalette butpal = b->palette();
      QPalette pagepal = this->palette();
      QColor bg = butpal.color(QPalette::Normal,QPalette::Button);
      pagepal.setColor(QPalette::Window,bg);
      setPalette(pagepal);
      setAutoFillBackground(true);
      setFrameStyle(QFrame::Panel | QFrame::Raised);
    }

    QString t = b->text();
    if (t.indexOf(":")<0 && t.indexOf("...")<0)
      b->setText(t + "&nbsp;...");
  }
}

guiPage::~guiPage() {
  foreach (guiButton *b, buttons)
    delete b;
}

void guiPage::addSpace(PageBuildGeom &g, QDomElement doc) {
  g.vspace(doc);
}

void guiPage::addBreak(PageBuildGeom &g, QDomElement doc) {
  g.nextColumn(doc);
}

void guiPage::addButtonGroup(PageBuildGeom &g, QDomElement doc) {
  guiButtonGroup *bg = new guiButtonGroup(this);
  bg->build(g, doc);
  groups[bg->groupId()] = bg;
  foreach (QString s, bg->childIDs())
    groupedButton[s] = bg->groupId();
}

static QString itemID(QDomElement doc) {
  if (doc.hasAttribute("id"))
    return doc.attribute("id");
  else if (doc.hasAttribute("custom"))
    return "custom-"+doc.attribute("custom");
  else if (doc.hasAttribute("value"))
    return doc.attribute("value");
  else
    return "";
}
  

guiItem *guiPage::createItem(QString) {
  throw Exception("guiPage", "Cannot create items");
}

guiButton *guiPage::addItem(PageBuildGeom &g, QDomElement doc) {
  guiButtonGroup *bg = groupp(":items");
  if (!bg) 
    groups[":items"] = bg = new guiButtonGroup(this);

  QString id = itemID(doc);
  if (id.isEmpty()) 
    throw Exception("guiPage", "Empty item ID in page " + path());
  
  guiButton *b = createItem(id);
  buttons[id] = b;
  buttons[id]->setup(doc);
  groupedButton[id] = ":items";

  b->setGeometry(g.bbox());
  g.advance();

  return b;
}

guiButton *guiPage::addButton(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc, "id",
			  "guiPage (addButton)", "Cannot read button ID");
  guiButton *b = new guiButton(this, id, master);
  b->setup(doc);
  buttons[id] = b;
  g.go(doc);
  b->setGeometry(g.bbox());
  if (!b->alwaysHidden())
    g.advance();

  return b;
}

void guiPage::addPage(PageBuildGeom &g, QDomElement doc,
			  Button::VisualType) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addPage)","Cannot read subpage ID");
  ParamTree *subtree = ptree->childp(id);
  // note that this will be NULL if we are a tabbed page

  guiPage *p = new guiPage(this, subtree, id, master, g.pbox());
  subPages[id] = p;
  p->setup(doc);
}

void guiPage::addTriangle(QString id) {
  triangle->activate(id.split("/").last());
}

void guiPage::removeTriangle(QString id) {
  triangle->deactivate(id.split("/").last());
}

void guiPage::paintEvent(class QPaintEvent *e) {
  QFrame::paintEvent(e);
  triangle->render();
}

void guiPage::addTabbedPage(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addTabbedPage)","Cannot read subpage ID");
  ParamTree *subtree = ptree->childp(id);
  // note that this will be NULL if we are a tabbed page

  guiPage *p = new guiTabbedPage(this, subtree, id, master, g.pbox());
  subPages[id] = p;
  p->setup(doc);
}

void guiPage::addMenu(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addMenu)","Cannot read subpage ID");
  ParamTree *subtree = ptree->childp(id);
  // note that this will be NULL if we are a tabbed page

  guiPage *p = new guiMenu(this, subtree, id, master, g.pbox());
  subPages[id] = p;
  p->setup(doc);
}

void guiPage::addChecklist(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addChecklist)","Cannot read subpage ID");
  ParamTree *subtree = ptree->childp(id);
  // note that this will be NULL if we are a tabbed page

  guiPage *p = new guiChecklist(this, subtree, id, master, g.pbox());
  subPages[id] = p;
  p->setup(doc);

  for (QMap<QString,guiButton *>::iterator i=p->buttons.begin();
       i!=p->buttons.end(); ++i) {
    guiButton *b = i.value();
     if (isType<guiItem>(b)) {
      connect(b,SIGNAL(selected(QString,QString)),
	      this,SLOT(childItemSelected(QString,QString)));
      connect(b,SIGNAL(deselected(QString,QString)),
	      this,SLOT(childItemDeselected(QString,QString)));
    }
  }
}

void guiPage::open() {
  emit opening(pathInstantiate(myPath),(QWidget*)(this));
  Param *pp = ptree->leafp();
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
    foreach (guiButton *b, buttons) {
      if (isType<guiItem>(b)) {
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

	b->setEnabled(master->canSetParam(master->pathInstantiate(myPath),
					  b->getValue()));
      } catch (Exception) {
	if (isType<guiItem>(b))
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
  if (id==triangle->currentId())
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
  if (subPages.contains(child)) {
    QString but = child + "*" + subPages[child]->getCurrentElement();
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
    } else if (isType<guiChecklist>(subPages[child])) {
      master->setParam(parpath, "+" + value);
    } else {
      master->setParam(parpath,value);
      buttons[child]->setValue(value);
    }
  }
  updateEnabled();
}

void guiPage::childItemDeselected(QString path, QString) {
  QString parpath = master->pathInstantiate(path);
  QStringList plist = parpath.split('/');
  QString value = plist.takeLast(); // That's the value, not the realpath
  parpath = plist.join("/");

  QString local=pathToLocal(path);
  QString child = local.left(local.indexOf('/'));

  guiButton *b=findpButton(local.split('/'));
  if (b) {
    if (isType<guiChecklist>(subPages[child])) {
      master->setParam(parpath,"-"+value);
      //ptree->find(pathToLocal(parpath)).report();
    }
  }
}

void guiPage::childItemCustomized(QString path, int cno, QString text) {
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
  setAutoFillBackground(true);
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  QString bg = doc.attribute("bg");
  if (!bg.isEmpty()) {
    QPalette p=palette();
    p.setColor(QPalette::Window, QColor(bg));
    setPalette(p);
  }
  QString bbg = doc.attribute("bbg");
  if (!bbg.isEmpty()) {
    QPalette p=palette();
    p.setColor(QPalette::Button, QColor(bbg));
    setPalette(p);
  }
}

void guiPage::addChildren(PageBuildGeom &g, QDomElement doc) {
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
      addMenu(g,e);
    else if (tag=="checklist")
      addChecklist(g,e);
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

guiPage const *guiPage::subpagep(QString id) const {
  return dynamic_cast<guiPage const *>(AbstractPage::subpagep(id));
}

guiPage *guiPage::subpagep(QString id) {
  return dynamic_cast<guiPage *>(AbstractPage::subpagep(id));
}

guiButtonGroup const *guiPage::groupp(QString id) const {
  if (groups.contains(id))
    return groups[id];
  else
    return 0;
}

guiButtonGroup *guiPage::groupp(QString id) {
  if (groups.contains(id))
    return groups[id];
  else
    return 0;
}

guiPage *guiPage::findpPage(QStringList path) {
  return dynamic_cast<guiPage *>(AbstractPage::findpPage(path));
}

guiPage &guiPage::findPage(QStringList path) {
  return dynamic_cast<guiPage &>(AbstractPage::findPage(path));
}

Button::VisualType guiPage::visualTypeForParentButton() const {
  return Button::VTPageOpen;
}

guiPage *guiPage::parentPage() {
  return dynamic_cast<guiPage *>(parent());
}
