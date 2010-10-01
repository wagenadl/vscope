// paramtree.cpp

#include <xml/paramtree.h>
#include <base/exception.h>
#include <xml/attribute.h>
#include <xml/xmlfind.h>
#include <xml/enumerator.h>
#include <QStringList>
#include <stdio.h>

ParamTree::ParamTree(QDomElement doc): base(doc) {
  leaf_ = 0;
  arrayElement = "";
  if (base.tagName()=="vsdscopeDefs") {
    base=base.firstChildElement("params");
    if (base.isNull())
      throw Exception("ParamTree","No <params> found.");
  }
  if (base.tagName()=="param") {
    // this is a leaf
    leaf_ = new Param(base);
#if PARAM_DBG
    leaf_->dbgPath = base.attribute("id");
#endif
  } else if (base.tagName()=="category" || base.tagName()=="params") {
    for (QDomElement e=base.firstChildElement();
	 !e.isNull(); e=e.nextSiblingElement())
      if (e.tagName()=="category" || e.tagName()=="array" || e.tagName()=="param")
	children.insert(xmlAttribute(e,"id"), new ParamTree(e));
  } else if (base.tagName()=="array") {
    // this is an array, we will construct elements
    QStringList eltnames = Enumerator::find(xmlAttribute(base,"enum"))->getAllTags();
    for (QStringList::iterator i=eltnames.begin(); i!=eltnames.end(); ++i) 
      children.insert(*i, new ParamTree(base,*i));
  } else {
    throw Exception("ParamTree",QString("Unexpected '" + base.tagName() + "' element."),"constructor");
  }
  buildEnablers();
#if PARAM_DBG
  if (base.tagName()=="params")
    setDbgPath("");
#endif
}

#if PARAM_DBG
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
#endif

ParamTree::ParamTree(QDomElement doc, QString elt): base(doc) {
  // this private constructor requires that doc is <array>
  leaf_=0;
  arrayElement = elt;
  for (QDomElement e=base.firstChildElement();
       !e.isNull(); e=e.nextSiblingElement())
    if (e.tagName()=="category" || e.tagName()=="array" || e.tagName()=="param")
      children.insert(xmlAttribute(e,"id"), new ParamTree(e));
  buildEnablers();
#if PARAM_DBG
  dbgPath = base.attribute("id") + "/" + dbgPath;
#endif
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
	throw Exception("ParamTree", "Param '"+e.attribute("id") + "' not found!?","buildEnablers");
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
	  throw Exception("ParamTree","Cond '"+cond+"' not found","buildEnablers");
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
      || doc.tagName()=="array" || doc.tagName()=="elt") {
    for (QDomElement e=doc.firstChildElement();
	 !e.isNull(); e=e.nextSiblingElement()) {
      ParamTree *c = childp(xmlAttribute(e,"id"));
      if (c) {
	try {
	  c->read(e);
	} catch(Exception const &) {
	  QString v = xmlAttribute(e,"value");
	  QString id = xmlAttribute(e,"id");
	  fprintf(stderr,"Warning: ParamTree (read): Could not assign %s to '%s'",
		  qPrintable(v),
		  qPrintable(id));
	}
      } else
	fprintf(stderr,"Warning: ParamTree (read): Unexpected element '%s' (type=%s). Ignoring.\n",
		qPrintable(xmlAttribute(e,"id")),qPrintable(e.tagName()));
    }
  } else {
    throw Exception("ParamTree",QString("Cannot read <") + doc.tagName() + "> elements.");
  }
}

void ParamTree::write(QDomElement doc) {
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
  
  for (QMap<QString,ParamTree*>::iterator i=children.begin();
       i!=children.end(); ++i) {
    ParamTree *c = i.value();
    QString id = i.key();
    if (id.startsWith("_"))
      continue; // don't write anything starting with "_".
    //fprintf(stderr,"  -> writing '%s' (%s)\n",qPrintable(id),c->leaf_?"leaf":"tree");
    if (c->leaf_) {
      // Child is a leaf, we should write a pval.
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
	//fprintf(stderr,"About to append leaf <%s> '%s' to <%s> '%s'\n",
	//	qPrintable(e.tagName()),qPrintable(e.attribute("id")),
	//	qPrintable(doc.tagName()),qPrintable(doc.attribute("id")));
	doc.appendChild(e);
      }
    } else {
      /* Child is not a leaf, we should write a category, array, or elt
	 depending on what kind of beast it is and let the child fill it.
      */
      QString childType = c->base.tagName();
      if (childType=="array" && !c->arrayElement.isEmpty())
	childType="elt";
      //fprintf(stderr,"  child is <%s> '%s'\n",qPrintable(childType),qPrintable(c->base.attribute("id")));
      bool found=false;
      for (QDomElement e=doc.firstChildElement(childType); !e.isNull();
	   e=e.nextSiblingElement()) {
	if (xmlAttribute(e,"id")==id) {
	  //fprintf(stderr,"-> will write to <%s> '%s'\n",qPrintable(e.tagName()),qPrintable(id));
	  c->write(e);
	  found=true;
	  break;
	}
      }
      if (!found) {
	QDomElement e = doc.ownerDocument().createElement(childType);
	e.setAttribute("id",id);
	//fprintf(stderr,"About to append tree <%s> '%s' to <%s> '%s'\n",
	//	qPrintable(e.tagName()),qPrintable(e.attribute("id")),
	//	qPrintable(doc.tagName()),qPrintable(doc.attribute("id")));
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

Param *ParamTree::leafp() {
  return leaf_;
}

Param &ParamTree::leaf() {
  if (leaf_)
    return *leaf_;
  else
    throw Exception("ParamTree: No leaf");
}

ParamTree *ParamTree::childp(QString id) {
  QMap<QString,ParamTree *>::iterator i = children.find(id);
  if (i==children.end())
    return 0;
  else
    return i.value();
}

ParamTree &ParamTree::child(QString id) {
  ParamTree *c = childp(id);
  if (c)
    return *c;
  else
    throw Exception(QString("ParamTree: No child named '") + id + "'");
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

//Param *ParamTree:findpCustom(QString path, int cno) {
//  QStringList p=path.split(QRegExp("[/:]"));
//  QString head = p.takeFirst();
//  if (p.isEmpty()) {
//    // we're at the level where the parameter should live
//    QString cust = QString("%1-%2").arg(head).arg(cno);
//    Param *pc = findp(cust);
//    if (pc)
//      return pc;
//    // ok, so we need to create it.
//    ParamTree *pt = new ParamTree
//    else 
//  } else {
//    // we should go further down
//    QString tail = p.join("/");
//    ParamTree *c = childp(head);
//    if (c)
//      return c->findpCustom(tail,cno);
//    else
//      return 0;
//  }
  
  

Param &ParamTree::find(QString path) {
  Param *p = findp(path);
  if (p)
    return *p;
  else
    throw Exception("ParamTree","Cannot find parameter '"+path+"'","find");
}

int ParamTree::report(int level, int maxelts) {
  if (level==0)
    printf("ParamTree report:\n");
  else
    printf("\n");
  if (leaf_) {
#if PARAM_DBG
    for (int l=0; l<level; l++)
      printf("  ");
    printf("%s: ", qPrintable(dbgPath));
#endif
    leaf_->report();
    maxelts--;
  } else {
    printf("\n");
  	   
    for (QMap<QString,ParamTree*>::iterator i=children.begin();
         i!=children.end(); ++i) {
      ParamTree *c = i.value();
      QString id = i.key();
  
      for (int l=0; l<level; l++)
        printf("  ");
#if PARAM_DBG
      printf("%s: ", qPrintable(dbgPath));
#else
      printf("%s: ", qPrintable(id));
#endif
      maxelts = c->report(level+1,maxelts);
      if (maxelts<=0)
	break;
    }
  }
  if (level==0)
    printf("End of ParamTree report.\n");
  return maxelts;
}

//uint64_t ParamTree::checksum() {
//  uint64_t sum=0;
//  throw Exception("ParamTree","checksum() not yet implemented");
//  return sum;
//}
