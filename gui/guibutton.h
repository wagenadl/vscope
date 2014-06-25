// guibutton.h

#ifndef GUIBUTTON_H

#define GUIBUTTON_H

#include <gfx/button.h>
#include <QDomElement>
#include <QString>

class guiButton: public Button {
  /*:C guiButton
   *:D One button (or a menu item) in an guiPage
   */
  Q_OBJECT;
public:  
  guiButton(class guiPage *parent, QString id, class guiRoot *master=0);
  /*:F constructor
   *:A parent: the parent widget. This may be an guiPage, but it
               doesn't need to be.
       id: name of this button relative to parent
   *:N Caller should position us afterwards.
   */
  virtual void setup(QDomElement doc);
  virtual ~guiButton();
  /*:F destructor
   */
  void setValue(QString v);
  /*:F setValue
   *:D Sets a new value for this button, formatting it as per the label
       attribute.
  */
  QString getValue() const { return value; }
  /*:F getValue
   *:D Returns the current value of the button.
   */
  void setFormat(QString fmt);
  /*:F setFormat
   *:D Sets a new format for this button. Typically, formats are of the
       form "Some text:\n%1". The "%1" is then replaced by the value.
  */
  QString getFormat() const { return format; }
  /*:F getFormat
   *:D Returns the current format for this button.
   */
  bool isCustom() const { return custom>0; }
  /*:F isCustom
   *:D Returns true iff this button has a "custom" attribute.
   *:N More precisely, iff that attribute has non-zero value.
   */
  int customNo() const { return custom; }
  /*:F customNo
   *:D Returns the integer value of the "custom" attribute or zero.
   */
  virtual bool hideWhenDisabled() const { return hidewhendisabled; }
  /*:F hideWhenDisabled
   *:D Returns whether or not this button hides itself when disabled.
   */
  void ensureValueInLabel();
  void dropValueFromLabel();
  /*:F ensureValueInLabel, dropValueFromLabel
   *:D For use by guiPage: ensure that the label contains "%1" so that
       menu values can be represented, or remove that %1.
  */
public:
  QString id() const { return myId; }
  /*:F id
   *:D Returns the leaf ID of this button.
   *:N Note that the ID according to gfx/Button's getID() is the path,
       not the leaf ID.
   */
  QString path() const;
  /*:F path
   *:D Returns the full path of this button.
   *:N This is identical to getID().
   */
  bool alwaysHidden() const;
protected:
  virtual void mousePressEvent(class QMouseEvent *);
  virtual void mouseDoubleClickEvent(class QMouseEvent *);
  /*:F mouseDoubleClickEvent
   *:D This pops up an edit box for custom buttons only.
   */
  virtual void mouseReleaseEvent(class QMouseEvent *);
  /*:F mouseClickEvent
   *:D This pops up an edit box for custom buttons only.
   */
  virtual void changeEvent(class QEvent *);
  /*:F changeEvent
   *:D Used to implement the "hideWhenDisabled" behavior.
   */
public slots:
  virtual void makeReadOnly(bool ro);
protected slots:
  virtual void editDone(QString newtext);
  /*:F editDone
   *:D This slot should be called when editing is complete.
   */
  virtual void editCanceled();
  /*:F editCanceled
   *:D This slot should be called when editing is canceled.
   */
  virtual void validate(QString newtext);
  /*:F validate
   *:D This slot should be called whenever the text is changed by the user.
   */
public slots:
  void closeEditor();
  /*:F closeEditor
   *:D This slot closes any open editor.
   */
  virtual void setEnabled(bool);
  virtual void setSelected(bool);
signals:
  void customize(QString mypath, int cno, QString newtext);
  /*:S customize
   *:D Emitted when a custom button is customized by the user.
   *:N Not emitted from setValue().
   *:N The new value has not been written to the button's text: you must
       connect this to some slot which checks whether the newtext is an
       acceptable value and if so, calls setValue().
   *:N If the value is not acceptable, the receiver can choose whether or
       not to close the editor by calling closeEditor().
   */
private:
  void openEditor(); // opens the TextEntry widget to edit this item.
protected:
  QString myId;
  QString format;
  QString value;
  QString editCaption;
  class TextEntry *editor;
  class guiRoot *master;
  int custom;
  class guiPage *parent;
  bool hidewhendisabled;
  bool alwayshidden;
  bool neverdisable;
protected:
  virtual void connectUp(QDomElement doc);
  virtual void stylize(QDomElement doc);
  /*:F connectUp, stylize
   *:N Descendents should *not* call these in their implementation.
   */
};

#endif
