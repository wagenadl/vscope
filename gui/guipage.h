// guipage.h

#ifndef GUIPAGE_H

#define GUIPAGE_H

#include <QFrame>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QDomElement>
#include <gfx/button.h>
#include <gui/abstractpage.h>
#include <gui/pagebuildgeom.h>
#include <gui/guibutton.h>

#define ARRAYSEP ":"

class guiPage: public AbstractPage {
  Q_OBJECT;
  /*:C guiPage
   *:D Represents a single page of buttons.
   */
public:
  guiPage(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  /*:F constructor
   *:D Constructs this page. Call setup() to construct the children.
   *:A parent: the parent window of this page (the application's top level,
               or a parent page).
    :  ptree:  the portion of the applications parameter tree that pertains
               to this page.
    :  doc:    a <page>, <checklist>, or <menu> element that defines this
               page and its children.
    :  master: the guiRoot to which all selections should be reported.
    :  mypath: the full path to this page ("" for root").
    :  geom:   position and size of this page relative to parent.
   */
  void setup(QDomElement doc);
  virtual ~guiPage();
  /*:F destructor
   *:D Destructs this page and all of its children.
   */
  guiPage *findpPage(QStringList path);
  guiPage &findPage(QStringList path);
  /*:F findpPage, findPage
   *:D As in AbstractPage, but returns pointer to guiPage
   */
  virtual guiButton const *buttonp(QString id) const;
  virtual guiButton *buttonp(QString id);
  virtual guiPage const *subpagep(QString id) const;
  virtual guiPage *subpagep(QString id);
  virtual class guiRadioGroup const *groupp(QString id) const;
  virtual class guiRadioGroup *groupp(QString id);
  virtual QList<guiButton *> allButtons();
public:
  class guiButton *addButton(PageBuildGeom &g, QDomElement elt);
  /*:F addButton
   *:D Adds a new button to this page.
   *:N This function also connects the button's selected/deselected/activated
       signals to the corresponding guiRoot's signals.
   */
  virtual class guiButton *addItem(PageBuildGeom &g, QDomElement elt);
  /*:F addItem
   *:D Adds a new item button to this page.
   *:N Items are automatically placed in a button group with radio-style
       selection for menus and toggle-style selection for checklists.
   *:N No connections are made to guiRoot signals.
   *:N The tail ID of the new button is: the id attribute of the <item>
       element if it has one, or else "#num" if the <item> has a custom="num"
       attribute, or else the value attribute of the <item>.
   *:N By "tail ID" I mean the part of the path after the last "/".
   */
  void addItems(PageBuildGeom &g, QDomElement elt);
  void addRadioGroup(PageBuildGeom &g, QDomElement elt);
  /*:F addRadioGroup
   *:D Adds a new group and any contained buttons to this page.
   */
  void addTabCtrl(PageBuildGeom &g, QDomElement elt);
  /*:F addTabCtrl
   *:D Adds a new group and any contained buttons to this page.
   */
  void addPage(PageBuildGeom &g, QDomElement elt,
	       VISUALTYPE vt=VT_PageOpen);
  /*:F addPage
   *:D Adds a new sub-page to this page.
   *:N This connects the selected/deselected signals from any button with the
       same name as this page to its open/close slots.
   */
  void addTabbedPage(PageBuildGeom &g, QDomElement elt);
  /*:F addTabbedPage
   *:D As addPage, but with added functionality for tabbed pages.
   */
  void addMenu(PageBuildGeom &g, QDomElement elt);
  /*:F addMenu
   *:D As addPage, but with added functionality for menus.
   *:N This connects the selected signal from menu items in that
       page to our childItemXXX slots.
   */
  void addChecklist(PageBuildGeom &g, QDomElement elt);
  /*:F addChecklist
   *:D As addPage, but with added functionality for checklists.
   *:N This connects the selected/deselected signals from items in that
       page to our childItemXXX slots.
   */
  virtual void addAuto(PageBuildGeom &g, QDomElement elt);
  /*:F addAuto
   *:D Automatically adds a number of <item> buttons according to an enum.
   */
  virtual bool deleteButton(QString id);
  /*:F deleteButton
   *:D Deletes a button and forgets about it.
   *:R True if the button was found.
   *:N A nonexistent button does not trigger an exception, just a "false"
       return code.
   */
public:
  guiPage *parentPage();
public slots:
  virtual void open();
  /*:F open
   *:D Opens this page, first updating all button texts based on the
       underlying ParamTree.
  */
  void close();
  /*:F close
   *:D Closes this page.
   */
  virtual void updateAuto();
  void childItemSelected(QString path, QString text);
  /*:F childItemSelected
   *:D Connect the selected() signal from subpage menu items to this.
   *:A path: full path of item that got selected (not instantiated).
    :  text: current text of that item.
   *:N This gets used for both checklists and menus.
   *:N E.g., if we are "acqEphys", connect "acqEphys/acqTime/10 s" to this.
   *:N A non-instantiated path is a path with the ":ELT" array element indicators
       absent.
   */
  void childItemDeselected(QString path, QString text);
  /*:F childItemDeselected
   *:D Connect the deselected() signal from subpage menu items to this.
   *:A path: full path of item that got deselected (not instantiated).
    :  text: current text of that item.
   *:N This gets used for checklists, not menus.
   *:N See childItemSelected for more notes.
   */
  void childItemCustomized(QString path, int customid, QString text);
  void childTabEnabled(QString path);
  void booleanButtonToggled(QString path);
protected:
  virtual void paintEvent(class QPaintEvent *event);
  virtual void prepForOpening();
  /*:F prepForOpening
   *:D Make us ready for being shown to the user.
   */
  virtual void openChildren();
private:
  void representTabEnabled(QString id);
  /*:F representTabEnabled
   *:D Visually indicates which tabs have their 'enabled' button on.
   *:A id: local ID of button (e.g. 'channel*DO3')
   */
  void setPageEnabled(bool on);
  /*:F setPageEnabled
   *:D Enables or shades all buttons in this page and subpages.
   */
  void updateEnableIfs();
  /*:F updateEnableIfs
   *:D Enables or shades buttons in this page depending on their "enable_if"
       status.
   *:N This also resets disabled values to their defaults.
  */
protected:
  QMap<QString, class guiButton *> buttons;
  /*:V buttons
   *:D All our buttons by ID. (ID is leaf, not full path).
   */
  QMap<QString, class guiRadioGroup *> groups;
  /*:V groups
   *:D All our radio groups / tab control groups by ID. (ID is leaf, not
       full path).
   */
  bool neverOpened;
  /*:V neverOpened
   *:D True if this page has never yet been opened.
   */
  bool pageEnabled;
  /*:V pageEnabled
   *:D True if this page is enabled (by its Enable button or its parent)
   */
private slots:
  void addTriangle(QString id);
  void removeTriangle(QString id);
private:
  void setDefaultColors(QDomElement);
  void addChildren(PageBuildGeom &, QDomElement);
  PageBuildGeom buildGeom;
  class guiTriangle *triangle;
  QRect origGeom;
  class RadioGroup *topgroup;
protected:
  virtual void connectToMaster(QDomElement doc);
  virtual void connectToParent(QDomElement doc);
  virtual void stylize(QDomElement doc);
  /*:F connectToMast, connectToParent, stylize
   *:N Implementers of the following should typically call base class's
       version first.
  */
  virtual QString getCurrentElement() const;
  virtual VISUALTYPE visualTypeForParentButton() const;
  virtual void sizeToFit();
  virtual class guiItem *createItem(QString id);
  virtual bool mayResize();
};

#endif
