// abstractpage.cpp

#include "abstractpage.h"

#include <xml/paramtree.h>
#include <xml/attribute.h>
#include <gui/guigeom.h>
#include <gui/guiroot.h>
#include <xml/enumerator.h>
#include <gui/guibutton.h>
#include <QRect>

#include <base/exception.h>
#include <base/dbg.h>

AbstractPage::AbstractPage(class QWidget *parent,
			   class ParamTree *ptree_,
			   QString id,
			   class guiRoot *master_,
			   class QRect const &geom): QFrame(parent) {
  prepped = false;
  setGeometry(geom);

  master = master_;
  ptree = origptree = ptree_;
  myId = id;
  AbstractPage *par = dynamic_cast<AbstractPage *>(parent);
  if (par) {
    myPath = par->myPath;
    if (myPath.isEmpty())
      myPath = id;
    else
      myPath += "/" + id;
  } else {
    myPath = id;
  }
}

AbstractPage::~AbstractPage() {
  for (QMap<QString, AbstractPage *>::iterator i=subPages.begin();
       i!=subPages.end(); ++i)
    delete i.value();
}

AbstractPage const *AbstractPage::subpagep(QString id) const {
  if (subPages.contains(id))
    return subPages[id];
  else
    return 0;
}

AbstractPage *AbstractPage::subpagep(QString id) {
  if (subPages.contains(id))
    return subPages[id];
  else
    return 0;
}

AbstractPage *AbstractPage::findpPage(QStringList path) {
  if (path.isEmpty())
    return this;
  QString head = path.takeFirst();
  AbstractPage *child = subpagep(head);
  if (child)
    return child->findpPage(path);
  else
    return 0;
}

AbstractPage &AbstractPage::findPage(QStringList path) {
  AbstractPage *pg = findpPage(path);
  if (pg)
    return *pg;
  else
    throw Exception("AbstractPage", "Cannot find page '" + path.join("/") + "'");
}

guiButton *AbstractPage::findpButton(QStringList path) {
  if (path.isEmpty())
    return 0;
  QString tail = path.takeLast();
  AbstractPage *page = findpPage(path);
  if (page)
    return page->buttonp(tail);
  else
    return 0;
}

guiButton &AbstractPage::findButton(QStringList path) {
  guiButton *b = findpButton(path);
  if (b)
    return *b;
  else
    throw Exception("AbstractPage", "Cannot find button '" + path.join("/") +"'");
}


QString AbstractPage::pathToLocal(QString path) const {
  QStringList list = path.split("/");
  QStringList us = myPath.split("/");
  QString elt = "";
  while (!us.isEmpty() && !myPath.isEmpty()) {
    QString ushead = us.takeFirst(); 
    QString themhead = list.takeFirst();
    int colonidx = themhead.indexOf(':');
    if (colonidx>=0) {
      if (us.isEmpty())
	elt = themhead.mid(colonidx);
      themhead = themhead.left(colonidx);
    }
    if (themhead!=ushead)
      throw Exception("AbstractPage","'"+path+"' is not a descendent of '"
		      +myPath+"'","pathToLocal");
  }
  if (!elt.isEmpty())
    list.push_front(elt);
  return list.join("/");
}

QString AbstractPage::pathToGlobal(QString leaf) const {
  QString path = myPath;
  if (!path.isEmpty() && !leaf.isEmpty() && leaf.left(1)!=":")
    path += "/";
  path+=leaf;
  return path;
}

QString AbstractPage::pathDeinstantiate(QString path) const {
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

QString AbstractPage::pathInstantiate(QString path) const {
  if (path=="")
    return "";
  
  QStringList pathBits = path.split("/");
  int myDepth = myPath.isEmpty() ? -1 : myPath.split("/").size()-1;

  if (pathBits.size()<=myDepth)
    return path;

  if (myDepth>=0) {
    QString elt = getCurrentElement();
    if (!elt.isEmpty()) {
      QString mybit = pathBits[myDepth];
      int colonidx = mybit.indexOf(':');
      if (colonidx>=0)
	pathBits[myDepth] = pathBits[myDepth].left(colonidx);
      pathBits[myDepth] += ":" + elt;
      path = pathBits.join("/");
    }
  }

  if (pathBits.size()<=myDepth+1)
    return path;
  
  QString child = pathBits[myDepth+1];
  int colonidx = child.indexOf(':');
  if (colonidx>=0)
    child=child.left(colonidx);
  if (subPages.contains(child))
    return subPages[child]->pathInstantiate(path);
  else if (buttonp(child))
    return path;
  else
    throw Exception("AbstractPage",
		    "'"+path+"' does not name a page or button in '" + myPath + "'",
		    "pathInstantiate");
}


void AbstractPage::setTree(ParamTree *neworigtree) {
  Dbg() << "retree: " << myPath << ": " << neworigtree;
  unprepare();
  if (neworigtree)
    origptree = neworigtree;

  if (!origptree)
    throw Exception("AbstractPage", "Cannot retree: no original tree", myPath);

  if (origptree->isArray()) {
    QString sub = getCurrentElement();
    ptree = sub=="" ? 0 : origptree->childp(sub);
    if (!ptree) {
      if (sub!="")
        Dbg() << "No ptree for " << getCurrentElement();
      else
        Dbg() << "No sub element";
      ptree = origptree;
      close();
      return;
    }
  } else {
    ptree = origptree;
  }

  foreach (QString id, subPages.keys())
    subPages[id]->setTree(ptree->childp(id));

  if (isVisible() && neworigtree!=0)
    open();
}

void AbstractPage::makeReadOnly(bool ro) {
  foreach (AbstractPage *pg, subPages)
    pg->makeReadOnly(ro);
}

void AbstractPage::unprepare() {
  prepped = false;
  foreach (AbstractPage *pg, subPages)
    pg->unprepare();
}
  
