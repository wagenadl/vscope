// xmlbutton.cpp

#include <gui/xmlbutton.h>
#include <xml/attribute.h>
#include <stdio.h>
#include <gfx/textentry.h>
#include <gui/xmlgui.h>
#include <QEvent>
#include <base/dbg.h>

xmlButton::xmlButton(class QWidget *parent,
		     QDomElement doc, QString mypath, xmlGui *master):
  Button(parent), master(master) {
  editor = 0;
  //fprintf(stderr,"xmlButton(%s)\n",qPrintable(mypath));
  if (doc.hasAttribute("custom"))
    custom=doc.attribute("custom").toInt(0);
  else if (doc.hasAttribute("editcaption"))
    custom = -1; // customizable, but not a numbered custom value
  else
    custom = 0;
  if (custom)
    editCaption = doc.attribute("editcaption");
  myPath = mypath;
  myTag = doc.tagName();
  setID(mypath);
  format = doc.hasAttribute("label") 
    ? doc.attribute("label")
    : "%1";
  if (doc.hasAttribute("bg"))
    setBackground(QColor(doc.attribute("bg")));
  if (doc.hasAttribute("value"))
    setValue(doc.attribute("value"));
  else
    setValue(doc.attribute("id"));
  if (custom>0) {
    QFont f = font();
    f.setItalic(true);
    setFont(f);
  }
  if (doc.hasAttribute("hidewhendisabled")) {
    Param p("bool");
    p.set(doc.attribute("hidewhendisabled"));
    hidewhendisabled = p.toBool();
  } else {
    hidewhendisabled = false;
  }
}

xmlButton::~xmlButton() {
  if (editor)
    delete editor;
}

void xmlButton::setFormat(QString f) {
  format=f;
  if (format.indexOf("%1")>=0)
    setText(format.arg(value));
  else
    setText(format);
}

void xmlButton::setValue(QString v) {
  value=v;
  if (format.indexOf("%1")>=0)
    setText(format.arg(value));
  else
    setText(format);
}

void xmlButton::mouseDoubleClickEvent(class QMouseEvent *e) {
  if (custom>0)
    openEditor();
  else
    Button::mouseDoubleClickEvent(e);
}

void xmlButton::mouseReleaseEvent(class QMouseEvent *e) {
  if (custom<0)
    openEditor();
  Button::mouseReleaseEvent(e);
}

void xmlButton::openEditor() {
  bool neweditor = !editor;
  if (neweditor) {
    QWidget *parent = this;
    while (parent->parentWidget())
      parent = parent->parentWidget();
    editor = new TextEntry(parent);
  }
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

void xmlButton::editDone(QString newtext) {
  emit customize(myPath,custom,newtext);
}

void xmlButton::validate(QString newtext) {
  if (master && editor) {
    QString parampath = master->pathInstantiate(myPath);
    if (custom>0)
      parampath = parampath.left(parampath.lastIndexOf('/'));
    bool ok =  master->canSetParam(parampath,newtext);
    editor->enable_ok(ok);
  }
}

void xmlButton::editCanceled() {
  if (editor)
    editor->hide();
}

void xmlButton::closeEditor() {
  if (editor)
    editor->hide();
}
  
void xmlButton::changeEvent(QEvent *e) {
  Button::changeEvent(e);
  //  dbg("xmlButton:changeEvent");
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
    
