// guibutton.cpp

#include <gui/guibutton.h>
#include <xml/attribute.h>
#include <stdio.h>
#include <gfx/textentry.h>
#include <gui/guiroot.h>
#include <QEvent>
#include <base/dbg.h>
#include <xml/aliases.h>
#include <base/exception.h>

guiButton::guiButton(QWidget *parnt, QString id, guiRoot *mastr):
  Button(parnt) {
  master = mastr;
  parent = dynamic_cast<guiPage *>(parnt); // this could be zero
  myId = id;
  setID(path());

  editor = 0;
  custom = 0;
  hidewhendisabled = false;
  alwayshidden = false;
}

void guiButton::setup(QDomElement doc) {
  if (doc.hasAttribute("custom"))
    custom=doc.attribute("custom").toInt(0);
  else if (doc.hasAttribute("editcaption"))
    custom = -1; // customizable, but not a numbered custom value

  if (custom)
    editCaption = doc.attribute("editcaption");

  format = doc.hasAttribute("label") 
    ? doc.attribute("label")
    : "%1";
  if (doc.hasAttribute("bg"))
    setBackground(QColor(doc.attribute("bg")));
  if (custom>0) {
    QFont f = font();
    f.setItalic(true);
    setFont(f);
  }

  if (doc.hasAttribute("value"))
    setValue(doc.attribute("value"));
  else
    setValue(doc.attribute("id"));

  if (doc.hasAttribute("hidewhendisabled")) {
    Param p("bool");
    p.set(doc.attribute("hidewhendisabled"));
    hidewhendisabled = p.toBool();
  }

  if (doc.hasAttribute("alwayshidden")) {
    Param p("bool");
    p.set(doc.attribute("alwayshidden"));
    alwayshidden = p.toBool();
  } 

  connectUp(doc);
  stylize(doc);
}

guiButton::~guiButton() {
  if (editor)
    delete editor;
}

void guiButton::setFormat(QString f) {
  format=f;
  if (format.indexOf("%1")>=0)
    setText(format.arg(value));
  else
    setText(format);
}

void guiButton::setValue(QString v) {
  value=v;
  if (format.indexOf("%1")>=0) {
    if (Aliases::has(v))
      v = Aliases::lookup(v);
    setText(format.arg(v));
  } else {
    setText(format);
  }
}

void guiButton::mouseDoubleClickEvent(class QMouseEvent *e) {
  if (custom>0)
    openEditor();
  else
    Button::mouseDoubleClickEvent(e);
}

void guiButton::mouseReleaseEvent(class QMouseEvent *e) {
  if (custom<0)
    openEditor();
  Button::mouseReleaseEvent(e);
}

static QWidget *grandParent(QWidget *p) {
  QWidget *q = p;
  while (q = p->parentWidget(), q)
    p = q;
  return p;
}

void guiButton::openEditor() {
  bool neweditor = !editor;
  if (neweditor)
    editor = new TextEntry(grandParent(this));
  editor->setText(format=="%1"?text():value);
  if (neweditor) {
    editor->setMessage(editCaption);
    editor->setAutoHide(false);
    connect(editor,SIGNAL(accept(QString)),this,SLOT(editDone(QString)));
    connect(editor,SIGNAL(cancel()),this,SLOT(editCanceled()));
    connect(editor,SIGNAL(edited(QString)),this,SLOT(validate(QString)));
  }
  editor->show();
}

void guiButton::editDone(QString newtext) {
  emit customize(path(), custom, newtext);
}

void guiButton::validate(QString newtext) {
  if (master && editor) {
    QString parampath = master->pathInstantiate(path());
    if (custom>0)
      parampath = parampath.left(parampath.lastIndexOf('/'));
    bool ok =  master->canSetParam(parampath,newtext);
    editor->enable_ok(ok);
  }
}

void guiButton::editCanceled() {
  if (editor)
    editor->hide();
}

void guiButton::closeEditor() {
  if (editor)
    editor->hide();
}
  
void guiButton::changeEvent(QEvent *e) {
  Button::changeEvent(e);
  //  dbg("guiButton:changeEvent");
  if (hidewhendisabled) {
    //dbg("  hidewhendisabled");
    if (e->type() == QEvent::EnabledChange) {
      //dbg("    enabled: %c", isEnabled() ? 'y':'n');
      if (isEnabled())
	show();
      else
	hide();
    }
  }
}
    
void guiButton::ensureValueInLabel() {
  if (!format.contains("%1")) {
    if (!format.endsWith(":"))
      format += ":";
    format += "<br>%1";
  }
}

QString guiButton::path() const {
  QString p = parent
    ? parent->path() + "/"
    : "";
  if (p=="/")
    p = "";
  p += id();
  return p;
}

void guiButton::connectUp(QDomElement doc) {
  if (doc.attribute("type") == "bool") {
    connect(this, SIGNAL(selected(QString,QString)),
	    parent, SLOT(booleanButtonToggled(QString)));
    connect(this, SIGNAL(deselected(QString,QString)),
	      parent, SLOT(booleanButtonToggled(QString)));
  } else if (custom<0) {
    connect(this, SIGNAL(customize(QString,int,QString)),
	    parent, SLOT(childItemCustomized(QString,int,QString)));
  } else {
    connect(this, SIGNAL(selected(QString,QString)),
	    master, SIGNAL(buttonSelected(QString,QString)));
    connect(this, SIGNAL(deselected(QString,QString)),
	    master, SIGNAL(buttonDeselected(QString,QString)));
    connect(this, SIGNAL(activated(QString,QString)),
	    master, SIGNAL(buttonClicked(QString,QString)));
    connect(this, SIGNAL(doubleClicked(QString,QString)),
	    master, SIGNAL(buttonDoubleClicked(QString,QString)));
  }
}

void guiButton::stylize(QDomElement doc) {
  if (custom) {
    makeRadio();
    setVisualType(VTVarValue);
  } else {
    makeAction();
    setVisualType(VTAction);
  }

  if (doc.attribute("type") == "bool") {
    QString txt = text();
    if (getSelected())
      txt.replace("Disabled", "Enabled");
    else
      txt.replace("Enabled", "Disabled");
    setText(txt);
    makeItem();
    setVisualType(Button::VTBooleanVar);
  } else if (doc.hasAttribute("type")) {
    throw Exception("guiButton", "Unknown type " + doc.attribute("type")
		    + " for button " + path());
  }
  if (alwayshidden)
    hide();
}
  
bool guiButton::alwaysHidden() const {
  return alwayshidden;
}
