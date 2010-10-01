// xmlmenu.cpp

#include <xml/xmlmenu.h>
#include <xml/attribute.h>
#include <gfx/button.h>
#include <base/exception.h>

#include <QDomElement>
#include <QPushButton>

#include <iostream>
#include <fstream>
using namespace std;

#include <QPalette>
#include <QColor>

xmlMenu::xmlMenu(QWidget *parent, class xmlPage *page): QFrame(parent), parentPage(page) {
  valueType = "";
  attached = false;
  setGeometry(MENU_MenuXL,MENU_MenuYT,MENU_MenuW,MENU_MenuH);  
}

void xmlMenu::populate(QDomElement menu, QDomElement distConfig) {
  myID=xmlAttribute(menu,"id","xmlMenu","No ID for menu");
  caption = xmlAttribute(menu,"caption");
  valueType = xmlAttribute(menu,"type","xmlMenu","No type for menu's values");
  if (valueType=="enum")
    valueEnum = xmlAttribute(menu,"enum","xmlMenu","No enum defined for menu's values");
  currentValue.newType(valueType,valueEnum);

  int nitems=0; // not sure we need this
  double nrows=0; // or this
  double ncols=0; // or this
  double row=0;
  double col=0;

  for (QDomElement e=menu.firstChildElement();
       !e.isNull();
       e=e.nextSiblingElement()) {
    if (e.tagName()=="item") {
      int x = int(MENU_MarginLX+(BUTTON_Width+MENU_MarginIX)*col);
      int y = int(MENU_MarginTY+(BUTTON_Height+MENU_MarginIY)*row);
      bool show=true;
      if (xmlAttribute(e,"show")=="no")
	show = false;
      QString text = xmlAttribute(e,"value","xmlMenu","Menu item without a value");
      currentValue.set(text);
      if (show) {
	Button *button = new Button(this,x,y);
	button->setAutoDefault(false);
	button->setFocusPolicy(Qt::NoFocus);
	button->setText(text);
	QString tag = text;
	if (e.hasAttribute("custom")) {
	  int cno = e.attribute("custom").toInt();
	  tag = QString("::") + QString::number(cno);
	  QMap<int,Setting>::iterator i =
	    customValues.insert(cno,Setting(valueType,valueEnum));
	  (*i).set(text);
	}
	items[tag] = button;
	row++;
      }
      // XXX here we should handle the item's childnodes, esp. "affect".
    } else if (e.tagName()=="space") {
      row++;
    } else if (e.tagName()=="split") {
      if (row>=nrows)
	nrows=row;
      row=0;
      col++;
    } else {
      cerr << "Warning: xmlMenu: unknown element in menu definition" << endl;
    }
  }
  
  if (menu.hasAttribute("default"))
    setValueAsString(menu.attribute("default"));
}

void xmlMenu::attach(QDomElement userConfig0, QStringList const &path) {
  /* If path has length n, then configuration lives in:
       0   toplevel
       1   page with id=path[0]
       2   page with id=path[0], subpage with id=path[1]
       3+  undefined
  */
  userConfig = userConfig0;
  QDomElement mydomain;
  QDomNodeList settings = userConfig.elementsByTagName("settings");
  if (settings.isEmpty()) {
    // create new settings from scratch
    mydomain = userConfig.ownerDocument().createElement("settings");
    userConfig.appendChild(mydomain);
  } else {
    mydomain = settings.item(0).toElement();
  }
  if (path.size()>=1) {
    // find appropriate page
    bool pageFound = false;
    for (QDomElement e=mydomain.firstChildElement();
	 !e.isNull();
	 e=e.nextSiblingElement()) {
      if (e.tagName()=="page" && e.attribute("id")==path[0]) {
	// found the correct page
	mydomain = e;
	pageFound = true;
	break;
      }
    }
    if (!pageFound) {
      // we'll have to create our node
      QDomElement e = QDomDocument::createElement("page");
      e.setAttribute("id",path[0]);
      mydomain.appendChild(e);
      mydomain = e;
    }
    if (path.size()>=2) {
      // Find subpage
      bool subPageFound = false;
      for (QDomElement e=mydomain.firstChildElement();
	   !e.isNull();
	   e=e.nextSiblingElement()) {
        if (e.tagName()=="subpage" && e.attribute("id")==path[0]) {
	  // found the correct page
	  mydomain = e;
	  subPageFound = true;
	  break;
        }
      }
      if (!subPageFound) {
        // we'll have to create our node
        QDomElement e = QDomDocument::createElement("subpage");
        e.setAttribute("id",path[1]);
        mydomain.appendChild(e);
        mydomain = e;
      }
    }
  }
  // now, let's find our value
  bool foundParam = false;
  for (QDomElement e=mydomain.firstChildElement();
       !e.isNull();
       e=e.nextSiblingElement()) {
    if (e.tagName()=="param" && e.attribute("id")==myID) {
      param = e;
      foundParam = true;
      currentValue.set(param.attribute("value"));
      break;
    }
  }
  if (!foundParam) {
    QDomElement e = QDomDocument::createElement("param");
    e.setAttribute("id",myID);
    e.setAttribute("value",currentValue.toString());
  }
  attached=true;
}

void xmlMenu::detach() {
  attached=false;
}
