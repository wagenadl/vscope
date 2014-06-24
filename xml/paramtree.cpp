// paramtree.cpp

#include <xml/paramtree.h>
#include <base/exception.h>
#include <xml/attribute.h>
#include <xml/xmlfind.h>
#include <xml/enumerator.h>
#include <QStringList>
#include <QSet>
#include <stdio.h>
#include <base/xml.h>
#include <base/dbg.h>

ParamTree::ParamTree(QDomElement doc): base(doc) {
  construct();
}

void ParamTree::construct() {  
  leaf_ = 0;
  arrayElement = "";
  if (base.tagName()=="vscope") {
    base=base.firstChildElement("params");
    if (base.isNull())
      throw Exception("ParamTree","No <params> found.");
  }
  decideSavable(base);
  if (base.tagName()=="param") {
    // this is a leaf
    leaf_ = new Param(base);
    leaf_->dbgPath = base.attribute("id");
  } else if (base.tagName()=="category" || base.tagName()=="params") {
    // this is a category, we will construct named children
    for (QDomElement e=base.firstChildElement();
	 !e.isNull(); e=e.nextSiblingElement())
      if (e.tagName()=="category" || e.tagName()=="array"
          || e.tagName()=="param")
	children.insert(xmlAttribute(e,"id"), new ParamTree(e));
  } else if (base.tagName()=="array") {
    // this is an array, we will construct elements based on enum
    QStringList eltnames
      = Enumerator::find(xmlAttribute(base, "enum"))->getAllTags();
    for (QStringList::iterator i=eltnames.begin(); i!=eltnames.end(); ++i) 
      children[*i] = new ParamTree(base, *i);
  } else {
    throw Exception("ParamTree",
                    QString("Unexpected '" + base.tagName() + "' element."),
                    "constructor");
  }
  buildEnablers();
  if (base.tagName()=="params")
    setDbgPath("");
}

ParamTree::ParamTree(QDomElement doc, QString elt): base(doc) {
  // this private constructor requires that doc is <array>
  leaf_ = 0;
  decideSavable(doc);
  arrayElement = elt;
  for (QDomElement e=base.firstChildElement();
       !e.isNull(); e=e.nextSiblingElement())
    if (e.tagName()=="category" || e.tagName()=="array" || e.tagName()=="param")
      children.insert(xmlAttribute(e,"id"), new ParamTree(e));
  buildEnablers();
  dbgPath = base.attribute("id") + "/" + dbgPath;
}

void ParamTree::setDbgPath(QString p) {
  dbgPath = p;
  if (arrayElement.isEmpty()) 
    dbgPath += "/" + base.attribute("id");
  else 
    dbgPath += ":" + arrayElement;
  for (QMap<QString,ParamTree*>::iterator i=children.begin();
       i!=children.end(); ++i)
    i.value()->setDbgPath(dbgPath);
}

void ParamTree::buildEnablers() {
  if (leaf_)
    return;
  if (base.tagName()=="array" && arrayElement.isEmpty())
    return;

  for (QDomElement e=base.firstChildElement();
       !e.isNull(); e=e.nextSiblingElement()) {
    if (e.tagName()=="param" && e.hasAttribute("cond")) {
      ParamTree *pt=children[e.attribute("id")];
      if (!pt)
	throw Exception("ParamTree",
                        "Param '" + e.attribute("id") + "' not found!?",
                        "buildEnablers");
      Param *p=pt->leaf_;
      QString cond = e.attribute("cond");
      QString enable_if = e.attribute("enable_if");
      QStringList enable_ifs = enable_if.split(" "); 
      if (cond=="*index") {
	if (arrayElement.isEmpty())
	  p->setEnabler(0,"");
	else if (enable_ifs.contains(arrayElement))
	  p->setEnabler(0,"");
	else
	  p->setEnabler(0,"*never");
      } else {
	ParamTree *ct=children[cond];
	if (!ct)
	  throw Exception("ParamTree", "Cond '"+cond+"' not found",
                          "buildEnablers");
	p->setEnabler(ct->leaf_,enable_if);
      }
    }
  }
}

void ParamTree::read(QDomElement doc) {
  if (doc.tagName()=="vsdscopeSettings")
    doc = doc.firstChildElement("settings");
  if (doc.tagName()=="pval") {
    leaf().read(doc);
  } else if (doc.tagName()=="settings" || doc.tagName()=="category"
             || doc.tagName()=="elt") {
    for (QDomElement e=doc.firstChildElement();
	 !e.isNull(); e=e.nextSiblingElement()) {
      QString id = xmlAttribute(e, "id");
      ParamTree *c = childp(id);
      if (c) {
	try {
	  c->read(e);
	} catch(Exception const &) {
	  QString v = xmlAttribute(e,"value");
          Dbg() << "Warning: ParamTree (read): Could not assign "
                << v << " to " << id;
	}
      } else {
        Dbg() << "Warning: ParamTree (read): Unexpected element " << id;
      }
    }
  } else if (doc.tagName()=="array") {
    QSet<QString> oldChildren = QSet<QString>::fromList(children.keys());
    QSet<QString> newChildren;
    for (QDomElement e=doc.firstChildElement();
         !e.isNull(); e=e.nextSiblingElement()) {
      if (e.tagName()=="elt") {
        QString id = xmlAttribute(e, "id");
        newChildren << id;
        if (children[id]==0) {
          Dbg() << "Paramtree: Old was zero " << id
                << " *" << oldChildren.contains(id)
                << " in " << dbgPath;
          children[id] = new ParamTree(base, id);
        }
        try {
          children[id]->read(e);
	} catch(Exception const &) {
          Dbg() << "Warning: ParamTree (read): Could not read array elt "
                << id; 
	}
      } else {
        Dbg() << "Warning: ParamTree (read): Unexpected element "
              << e.tagName() << " as child of array";
      }
    }
    foreach (QString id, oldChildren) {
      if (!newChildren.contains(id)) {
        Dbg() << "paramtree: Dropping " << id
              << " in " << dbgPath;
        delete children[id];
        children.remove(id);
      }
    }    
  } else if (doc.tagName()=="elt") {
    for (QDomElement e=doc.firstChildElement();
	 !e.isNull(); e=e.nextSiblingElement()) {
      QString id = xmlAttribute(e, "id");
      ParamTree *c = childp(id);
      if (c) {
	try {
	  c->read(e);
	} catch(Exception const &) {
	  QString v = xmlAttribute(e,"value");
          Dbg() << "Warning: ParamTree (read): Could not assign "
                << v << " to " << id;
	}
      } else {
        Dbg() << "Warning: ParamTree (read): Unexpected element " << id;
      }
    }
  } else {
    throw Exception("ParamTree",QString("Cannot read <") + doc.tagName()
                    + "> elements.");
  }
}

void ParamTree::write(QDomElement doc) const {
  //fprintf(stderr,"ParamTree::write <%s> '%s' to <%s> '%s'\n",
  //        qPrintable(base.tagName()),qPrintable(base.attribute("id")),
  //        qPrintable(doc.tagName()),qPrintable(doc.attribute("id")));

  if (doc.tagName()=="vsdscopeSettings") {
    QDomElement e = doc.firstChildElement("settings");
    if (e.isNull()) {
      e = doc.ownerDocument().createElement("settings");
      doc.appendChild(e);
    }
    doc = e;
  }
  if (leaf_)
    // We are a leaf, this should not happen.
    throw Exception("ParamTree","write() cannot directly write leaves");
  
  for (QMap<QString, ParamTree *>::const_iterator i=children.begin();
       i!=children.end(); ++i) {
    ParamTree *c = i.value();
    QString id = i.key();
    if (!c->savable)
      continue; // don't write non-savable children.
    if (c->leaf_) {
      /* Child is a leaf, we should write a pval. */
      bool found=false;
      for (QDomElement e=doc.firstChildElement("pval"); !e.isNull();
	   e=e.nextSiblingElement()) {
	if (xmlAttribute(e,"id")==id) {
	  c->leaf_->write(e);
	  found=true;
	  break;
	}
      }
      if (!found) {
	QDomElement e = doc.ownerDocument().createElement("pval");
	e.setAttribute("id",id);
	c->leaf_->write(e);
	doc.appendChild(e);
      }
    } else {
      /* Child is not a leaf, we should write a category, array, or elt
	 depending on what kind of beast it is and let the child fill it.
      */
      QString childType = c->base.tagName();
      if (childType=="array" && !c->arrayElement.isEmpty())
	childType="elt";
      bool found=false;
      for (QDomElement e=doc.firstChildElement(childType); !e.isNull();
	   e=e.nextSiblingElement()) {
	if (xmlAttribute(e,"id")==id) {
	  c->write(e);
	  found=true;
	  break;
	}
      }
      if (!found) {
	QDomElement e = doc.ownerDocument().createElement(childType);
	e.setAttribute("id",id);
	doc.appendChild(e);
	c->write(e);
      }
    }
  }
}

void ParamTree::reset() {
  if (leaf_) {
    if (base.hasAttribute("default"))
      leaf_->set(base.attribute("default"));
    else
      fprintf(stderr,"Warning: ParamTree::reset: No default for '%s'.\n",
	      qPrintable(base.attribute("id")));
  }

  for (QMap<QString,class ParamTree *>::iterator i=children.begin();
       i!=children.end(); ++i) {
    i.value()->reset();
  }
}

Param const *ParamTree::leafp() const {
  return leaf_;
}

Param *ParamTree::leafp() {
  return leaf_;
}

Param &ParamTree::leaf() {
  if (leaf_)
    return *leaf_;
  else
    throw Exception("ParamTree: No leaf");
}

Param const &ParamTree::leaf() const {
  if (leaf_)
    return *leaf_;
  else
    throw Exception("ParamTree: No leaf");
}

ParamTree const *ParamTree::childp(QString id) const {
  if (children.contains(id))
    return children[id];
  else
    return 0;
}

ParamTree *ParamTree::childp(QString id) {
  if (children.contains(id))
    return children[id];
  else
    return 0;
}

ParamTree const &ParamTree::child(QString id) const {
  ParamTree const *c = childp(id);
  if (c)
    return *c;
  else
    throw Exception(QString("ParamTree: No child named '") + id + "'");
}

ParamTree &ParamTree::child(QString id) {
  ParamTree *c = childp(id);
  if (c)
    return *c;
  else
    throw Exception(QString("ParamTree: No child named '") + id + "'");
}

ParamTree const *ParamTree::treep(QString path) const {
  if (path.isEmpty())
    return this;
  QStringList p=path.split(QRegExp("[/:]"));
  QString head = p.takeFirst();
  QString tail = p.join("/");
  ParamTree const *c = childp(head);
  if (c)
    return c->treep(tail);
  else
    return 0;
}

ParamTree *ParamTree::treep(QString path) {
  if (path.isEmpty())
    return this;
  QStringList p=path.split(QRegExp("[/:]"));
  QString head = p.takeFirst();
  QString tail = p.join("/");
  ParamTree *c = childp(head);
  if (c)
    return c->treep(tail);
  else
    return 0;
}

ParamTree const &ParamTree::tree(QString path) const {
  ParamTree const *t = treep(path);
  if (t)
    return *t;
  else
    throw Exception("ParamTree", "No subtree named " + path,
		    dbgPath);
}

ParamTree &ParamTree::tree(QString path) {
  ParamTree *t = treep(path);
  if (t)
    return *t;
  else
    throw Exception("ParamTree", "No subtree named " + path,
		    dbgPath);
}


Param const *ParamTree::findp(QString path) const {
  if (path.isEmpty())
    return leafp();
  QStringList p=path.split(QRegExp("[/:]"));
  QString head = p.takeFirst();
  QString tail = p.join("/");
  ParamTree const *c = childp(head);
  if (c)
    return c->findp(tail);
  else
    return 0;
}

Param *ParamTree::findp(QString path) {
  if (path.isEmpty())
    return leafp();
  QStringList p=path.split(QRegExp("[/:]"));
  QString head = p.takeFirst();
  QString tail = p.join("/");
  ParamTree *c = childp(head);
  if (c)
    return c->findp(tail);
  else
    return 0;
}

Param const &ParamTree::find(QString path) const {
  Param const *p = findp(path);
  if (p)
    return *p;
  else
    throw Exception("ParamTree","Cannot find parameter '"+path+"'",
		    dbgPath);
}

Param &ParamTree::find(QString path) {
  Param *p = findp(path);
  if (p)
    return *p;
  else
    throw Exception("ParamTree","Cannot find parameter '"+path+"'",
		    dbgPath);
}

int ParamTree::report(int level, int maxelts) const {
  if (level==0)
    printf("ParamTree report:\n");
  else
    printf("\n");
  if (leaf_) {
    for (int l=0; l<level; l++)
      printf("  ");
    printf("%s: ", qPrintable(dbgPath));
    leaf_->report();
    maxelts--;
  } else {
    printf("\n");
  	   
    for (QMap<QString,ParamTree*>::const_iterator i=children.begin();
         i!=children.end(); ++i) {
      ParamTree *c = i.value();
      QString id = i.key();
  
      for (int l=0; l<level; l++)
        printf("  ");
      printf("%s: ", qPrintable(dbgPath));
      maxelts = c->report(level+1,maxelts);
      if (maxelts<=0)
	break;
    }
  }
  if (level==0)
    printf("End of ParamTree report.\n");
  return maxelts;
}

bool ParamTree::enabled(QString path) const {
  QStringList bits = path.split("/");
  QString p0 = "";
  while (!bits.isEmpty()) {
    p0 += bits.takeFirst();
    p0 += "/";
    QString p_en = p0 + "enable";
    Param const *p = findp(p_en);
    if (p && !p->toBool())
      return false;
  }
  return true;
}

ParamTree &ParamTree::operator=(ParamTree const &other) {
  XML xml(0,"vsdscopeSettings");
  other.write(xml.root());
  read(xml.root());
  return *this;
}

ParamTree::ParamTree(ParamTree const &other): base(other.base) {
  construct();
  XML xml(0,"vsdscopeSettings");
  other.write(xml.root());
  read(xml.root());
}

bool ParamTree::isSavable() const {
  return savable;
}

void ParamTree::setSavable(bool f) {
  savable = f;
}

void ParamTree::decideSavable(QDomElement e) {
    if (e.hasAttribute("save")) {
    Param s("bool");
    s.set(xmlAttribute(e, "save"));
    savable = s.toBool();
  } else {
    savable = true;
  }
}

bool ParamTree::isArray() const {
  return base.tagName()=="array";
}
