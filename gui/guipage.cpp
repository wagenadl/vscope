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
#include <QPainter>
#include <QPen>
#include <gfx/radiogroup.h>
#include <gui/guiradiogroup.h>
#include <gui/guitabctrl.h>
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
  buildGeom(this) {
  triangle = new guiTriangle(this);
  itemgroup = 0;
  topgroup = new RadioGroup(this);

  setLineWidth(0);
  neverOpened = true;
  pageEnabled = true; // reasonable default?

  hide();
}

void guiPage::setup(QDomElement doc) {
  setDefaultColors(doc);
  buildGeom.setup(doc);
  addChildren(buildGeom, doc);
  connectToMaster(doc);
  connectToParent(doc);
  stylize(doc);
  origGeom = geometry();
  sizeToFit();
}

bool guiPage::mayResize() {
  foreach (guiTabCtrl *g, tabctrls)
    if (!g->mayResize())
      return false;
  return true;
}

void guiPage::sizeToFit() {
  if (!mayResize())
    return;
  QRect b0 = QRect(0, 0, origGeom.width(), origGeom.height());
  QRect bb = buildGeom.boundingBox();
  //  Dbg() << "guiPage["<<path()<<"]: sizetofit b0="<<b0<<"; bb="<<bb;
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

void guiPage::connectToParent(QDomElement) {
  guiPage *par = parentPage();
  if (!par)
    return;
  guiButton *b = par->buttonp(id());
  if (!b)
    return;

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
}

void guiPage::stylize(QDomElement doc) {
  guiPage *par = parentPage();
  if (!par)
    return;
  guiButton *b = par->buttonp(id());
  if (!b)
    return;

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

guiPage::~guiPage() {
  // buttons and other descendents will be deleted by qt
}

void guiPage::addRadioGroup(PageBuildGeom &g, QDomElement doc) {
  guiRadioGroup *bg = new guiRadioGroup(this);
  bg->build(g, doc);
  groups[doc.attribute("id")] = bg;
}

void guiPage::addTabCtrl(PageBuildGeom &g, QDomElement doc) {
  guiTabCtrl *bg = new guiTabCtrl(this);
  bg->build(g, doc);
  tabctrls[doc.attribute("id")] = bg;
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
  if (!itemgroup)
    itemgroup = new RadioGroup(this);

  QString id = itemID(doc);
  if (id.isEmpty()) 
    throw Exception("guiPage", "Empty item ID in page " + path());
  
  guiButton *b = createItem(id);
  buttons[id] = b;
  buttons[id]->setup(doc);
  itemgroup->add(b);

  if (doc.hasAttribute("custom"))
    g.last(doc.attribute("custom").toInt());
  b->setGeometry(g.bbox());
  g.right();
  b->show();

  return b;
}

void guiPage::addItems(PageBuildGeom &g, QDomElement doc) {
  QDomDocument xml;
  QString hd1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone='yes'?>";
  QString hd2 = "<!DOCTYPE vscopeAuto>";
  foreach (QString v, doc.text().split(QRegExp("[,\n]+\\s*"))) {
    if (v=="")
      continue;
    xml.setContent(hd1 + "\n" + hd2 + "\n"
		   + "<item value=\""
		   + v.simplified()
		   + "\"/>\n");
    QDomElement e = xml.documentElement();
    addItem(g, e);
  }
}

guiButton *guiPage::addButton(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc, "id",
			  "guiPage (addButton)", "Cannot read button ID");
  guiButton *b = new guiButton(this, id, master);
  topgroup->add(b);
  b->setup(doc);
  buttons[id] = b;
  g.go(doc);
  b->setGeometry(g.bbox());
  if (!b->alwaysHidden())
    g.down();

  return b;
}

void guiPage::addPage(PageBuildGeom &g, QDomElement doc,
		      VISUALTYPE) {
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
  ParamTree *subtree = ptree ? ptree->childp(id) : 0;
  Dbg() << "addTabbedPage: " << path() << " : " << id;
  // note that this will be NULL if we are a tabbed page

  guiPage *p = new guiTabbedPage(this, subtree, id, master, g.pbox());
  subPages[id] = p;
  p->setup(doc);
}

void guiPage::addMenu(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addMenu)","Cannot read subpage ID");
  ParamTree *subtree = ptree ? ptree->childp(id) : 0;
  // note that this will be NULL if we are a tabbed page

  guiPage *p = new guiMenu(this, subtree, id, master, g.pbox());
  subPages[id] = p;
  p->setup(doc);
}

void guiPage::addChecklist(PageBuildGeom &g, QDomElement doc) {
  QString id=xmlAttribute(doc,"id",
			  "guiPage (addChecklist)","Cannot read subpage ID");
  ParamTree *subtree = ptree ? ptree->childp(id) : 0;
  // note that this will be NULL if we are a tabbed page

  guiPage *p = new guiChecklist(this, subtree, id, master, g.pbox());
  subPages[id] = p;
  p->setup(doc);
}

void guiPage::open() {
  emit opening(pathInstantiate(myPath), (QWidget*)(this));
  Dbg() << "guiPage:"<<myPath<<": open first=" << neverOpened;

  Param *p = ptree->findp("enable");
  setPageEnabled(p ? p->toBool() : true);

  if (neverOpened) {
    foreach (guiRadioGroup *bg, groups) 
      bg->selectDefaultButton();
  }

  openChildren();

  neverOpened = false;
  show();

  emit opened(pathInstantiate(myPath), (QWidget*)(this));
}

void guiPage::openChildren() {
  foreach (QString id, subPages.keys()) {
    if (buttons.contains(id)) {
      guiButton *b = buttons[id];
      if (b->getSelected())
	subPages[id]->open();
    }
  }
}

void guiPage::prepForOpening() {
  foreach (guiTabCtrl *g, tabctrls)
    g->rebuild();
  
  foreach (QString id, buttons.keys()) {
    guiButton *b = buttons[id];
    Param *p = ptree ? ptree->findp(id) : 0;
    if (p) {
      // This is a button that directly represents a value.
      bool ena = p->isEnabled();
      if (!ena) {
	b->setEnabled(false);
	p->restore();
	guiPage *subpage = subpagep(id);
	if (subpage) {
	  subpage->hide();
	  b->setSelected(false);
	}
      }
      if (p->getType()=="bool") {
	b->makeToggle();
	b->setSelected(p->toBool());
      } else {
	b->setValue(p->toString());
      }
    } else if (id.indexOf(ARRAYSEP)>=0) {
      // This is a button that represents a tab.
      representTabEnabled(id);
    }
  }
}

void guiPage::representTabEnabled(QString id) {
  guiButton *b = buttons[id];
  int idx = id.indexOf(ARRAYSEP);
  if (idx<0)
    throw Exception("guiPage", "representTabEnabled: not a tab " + id);
  QString ar = id.left(idx);
  QString elt = id.mid(idx+1);
  Param *p = origptree ? origptree->findp(ar+"/"+elt+"/enable") : 0;
  if (p) {
    QPalette pa = b->palette();
    pa.setColor(QPalette::Mid,
		p->toBool() ? QColor("#ff2200") : QColor("#884444"));
    b->setPalette(pa);
    if (triangle->currentId()==id)
      triangle->recolor();
  }
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
  guiPage *p = subpagep(child);
  if (p) {
    QString but = child + ARRAYSEP + p->getCurrentElement();
    representTabEnabled(but);
  } else {
    throw Exception("guiPage", "childTabEnabled can't find child " + path);
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
  QString subpath = pathToLocal(path);
  guiButton *b = buttonp(subpath);
  if (!b)
    throw Exception("guiPage", "booleanButtonToggled should not be triggered from nonlocal buttons " + this->path() + " -> " + path);
  
  bool on = b->getSelected();
  master->setParam(parpath, on ? "yes" : "no");
  if (subpath=="enable") 
    setPageEnabled(on);
  else 
    updateEnableIfs();

}

void guiPage::updateEnableIfs() {
  foreach (QString id, buttons.keys()) {
    Param *p = ptree ? ptree->findp(id) : 0;
    if (p) {
      bool ena = p->isEnabled();
      buttons[id]->setEnabled(ena && pageEnabled);
      if (!ena) 
	buttons[id]->setValue(p->toString());
    }
  }
}  

void guiPage::setPageEnabled(bool enable) {
  pageEnabled = enable;
  foreach (guiButton *b, buttons)
    b->setEnabled(enable);

  updateEnableIfs();

  foreach (AbstractPage *p0, subPages) {
    guiPage *p = dynamic_cast<guiPage *>(p0);
    if (p->isVisible())
      p->setPageEnabled(enable);
  }
  
  prepForOpening();
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
  updateEnableIfs();
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

void guiPage::addAuto(PageBuildGeom &, QDomElement) {
  throw Exception("guiPage", "Don't know how to addAuto. " + path());
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
      addRadioGroup(g,e);
    else if (tag=="tabctrl")
      addTabCtrl(g,e);
    else if (tag=="button")
      addButton(g,e);
    else if (tag=="item")
      addItem(g,e);
    else if (tag=="items")
      addItems(g,e);
    else if (tag=="tabbedpage")
      addTabbedPage(g,e);
    else if (tag=="page")
      addPage(g,e);
    else if (tag=="menu")
      addMenu(g,e);
    else if (tag=="checklist")
      addChecklist(g,e);
    else if (tag=="auto")
      addAuto(g, e);
    else
      fprintf(stderr,"Warning: guiPage: Unexpected tag <%s>.\n",
	      qPrintable(tag));
  }
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

guiTabCtrl const *guiPage::tabctrlp(QString id) const {
  if (tabctrls.contains(id))
    return tabctrls[id];
  else
    return 0;
}

guiTabCtrl *guiPage::tabctrlp(QString id) {
  if (tabctrls.contains(id))
    return tabctrls[id];
  else
    return 0;
}

guiRadioGroup const *guiPage::radiogroupp(QString id) const {
  if (groups.contains(id))
    return groups[id];
  else
    return 0;
}

guiRadioGroup *guiPage::radiogroupp(QString id) {
  if (groups.contains(id))
    return groups[id];
  else
    return 0;
}

guiPage const *guiPage::subpagep(QString id) const {
  return dynamic_cast<guiPage const *>(AbstractPage::subpagep(id));
}

guiPage *guiPage::subpagep(QString id) {
  return dynamic_cast<guiPage *>(AbstractPage::subpagep(id));
}

guiPage *guiPage::findpPage(QStringList path) {
  return dynamic_cast<guiPage *>(AbstractPage::findpPage(path));
}

guiPage &guiPage::findPage(QStringList path) {
  return dynamic_cast<guiPage &>(AbstractPage::findPage(path));
}

VISUALTYPE guiPage::visualTypeForParentButton() const {
  return VT_PageOpen;
}

guiPage *guiPage::parentPage() {
  return dynamic_cast<guiPage *>(parent());
}

QString guiPage::getCurrentElement() const {
  return "";
}

bool guiPage::deleteButton(QString id) {
  if (buttons.contains(id)) {
    delete buttons[id];
    buttons.remove(id);
    return true;
  } else {
    return false;
  }
}

QList<guiButton *> guiPage::allButtons() {
  return buttons.values();
}
