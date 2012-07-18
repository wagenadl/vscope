// abstractpage.cpp

#include "abstractpage.h"

#include <xml/paramtree.h>
#include <xml/attribute.h>
#include <gui/guigeom.h>
#include <gui/xmlgui.h>
#include <xml/enumerator.h>
#include <gui/xmlbutton.h>
#include <QRect>

#include <base/exception.h>
#include <base/dbg.h>

AbstractPage::AbstractPage(class QWidget *parent,
			   class ParamTree *ptree_,
			   QDomElement doc,
			   class xmlGui *master_,
			   QString mypath,
			   class QRect const &geom): QFrame(parent) {
  setGeometry(geom);

  master = master_;
  ptree = origptree = ptree_;
  myPath = mypath;
  myTag = doc.tagName();
}

void AbstractPage::addChildren(QDomElement doc) {
}

void AbstractPage::removeChildren() {
}

AbstractPage::~AbstractPage() {
  for (QMap<QString, xmlPage *>::iterator i=subPages.begin();
       i!=subPages.end(); ++i)
    delete i.value();
}

AbstractPage *AbstractPage::findpPage(QStringList path) {
  if (path.isEmpty())
    return this;
  QString head = path.takeFirst();
  if (subPages.contains(head))
    return subPages[head]->findpPage(path);
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

QString AbstractPage::pathToLocal(QString path) const {
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
      list[us.size()-1] += ":" + getCurrentElement();
  }
  if (list.size()>us.size()) {
    QString child = list[us.size()];
    int colonidx = child.indexOf(':');
    if (colonidx>=0)
      child=child.left(colonidx);
    if (subPages.contains(child))
      return subPages[child]->pathInstantiate(list.join("/"));
    else // if (buttons.contains(child))
      return list.join("/");
    //else
    //throw Exception("AbstractPage","'"+path+"' does not name a page or button","pathInstantiate");
  } else {
    return list.join("/");
  }
}

QString AbstractPage::getCurrentElement() const {
  /* This really should be called only on tabbed pages */
  return "";
}

