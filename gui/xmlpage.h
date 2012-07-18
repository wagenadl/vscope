// xmlpage.h

#ifndef XMLPAGE_H

#define XMLPAGE_H

#include <QFrame>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QDomElement>
#include <gfx/button.h>
#include <gui/abstractpage.h>

class xmlPage: public AbstractPage {
  Q_OBJECT;
  /*:C xmlPage
   *:D Represents a single page of buttons.
   */
public:
  xmlPage(class QWidget *parent,
	  class ParamTree *ptree,
	  QDomElement doc,
	  class xmlGui *master,
	  QString mypath,
	  class QRect const &geom);
  /*:F constructor
   *:D Constructs this page and all of its children.
   *:A parent: the parent window of this page (the application's top level,
               or a parent page).
    :  ptree:  the portion of the applications parameter tree that pertains
               to this page.
    :  doc:    a <page>, <checklist>, or <menu> element that defines this
               page and its children.
    :  master: the xmlGui to which all selections should be reported.
    :  mypath: the full path to this page ("" for root").
    :  geom:   position and size of this page relative to parent.
   */
  virtual ~xmlPage();
  /*:F destructor
   *:D Destructs this page and all of its children.
   */
  xmlPage *findpPage(QStringList path);
  xmlPage &findPage(QStringList path);
  /*:F findpPage, findPage
   *:D As in AbstractPage, but returns pointer to xmlPage
   */
  virtual xmlButton const *buttonp(QString id) const;
  virtual xmlButton *buttonp(QString id);
  virtual xmlButton &button(QString id);
  /*:F buttonp, button
   *:D Get pointer or reference to button that is an immediate child of our page.
   */
private:
  struct Geom {
    /*:C Geom
     *:D Information about geometry used while building this page.
     */
    double nextcol, nextrow; // location of next button
    struct ButtonPlacement {
      int x0, y0; // pixel position of button grid origin
      int w, h; // size of buttons
      double dx, dy; // grid spacing
    } button;
    struct SubPagePlacement {
      int dxl, dxr, dyt, dyb; // pixel count of how much smaller subpages compared to us.
    } page; 
    int rows, cols; // number of rows,cols of buttons, or 0 if not specified.
    int caph; // height of caption widget, or 0 if none.
  };
  class xmlButton *addButton(Geom &g, QDomElement elt);
  /*:F addButton
   *:D Adds a new button to this page.
   *:N This function also connects the button's selected/deselected/activated
       signals to the corresponding xmlGui's signals.
   */
  class xmlButton *addItem(Geom &g, QDomElement elt);
  /*:F addItem
   *:D Adds a new item button to this page.
   *:N Items are automatically placed in a button group with radio-style
       selection for menus and toggle-style selection for checklists.
   *:N No connections are made to xmlGui signals.
   *:N The tail ID of the new button is: the id attribute of the <item>
       element if it has one, or else "#num" if the <item> has a custom="num"
       attribute, or else the value attribute of the <item>.
   *:N By "tail ID" I mean the part of the path after the last "/".
   */
  void addButtonGroup(Geom &g, QDomElement elt);
  /*:F addButtonGroup
   *:D Adds a new button group and any contained buttons to this page.
   */
  class xmlPage *addPage(Geom &g, QDomElement elt,
			 Button::VisualType vt=Button::VTPageOpen);
  /*:F addPage
   *:D Adds a new sub-page to this page.
   *:N This connects the selected/deselected signals from any button with the
       same name as this page to its open/close slots.
   */
  class xmlPage *addTabbedPage(Geom &g, QDomElement elt);
  /*:F addTabbedPage
   *:D As addPage, but with added functionality for tabbed pages.
   */
  class xmlPage *addMenuPage(Geom &g, QDomElement elt);
  /*:F addMenuPage
   *:D As addPage, but with added functionality for menus.
   *:N This connects the selected signal from menu items in that
       page to our childItemXXX slots.
   */
  class xmlPage *addCheckListPage(Geom &g, QDomElement elt);
  /*:F addChecklistPage
   *:D As addPage, but with added functionality for checklists.
   *:N This connects the selected/deselected signals from items in that
       page to our childItemXXX slots.
   */
  void addCaption(Geom &g);
  /*:F addCaption
   *:D Adds a caption widget to a menu.
   *:N This uses the 'caption' member; if the caption is empty, it does
       not create a widget.
  */
  void addBreak(Geom &g, QDomElement elt);
  /*:F addBreak
   *:D Inserts a column break between buttons, as per the <break> element.
   */
  void addSpace(Geom &g, QDomElement elt);
  /*:F addSpace
   *:D Inserts a vertical space between buttons, as per the <space> element.
   */
  void addAuto(Geom &g, QDomElement elt);
  /*:F addAuto
   *:D Automatically adds a number of <item> buttons according to an enum.
   */
public:
  QList<class xmlButton *> getGroup(QString id) const;
  /*:F getGroup
   *:D Returns a list of all buttons in a given group.
   *:N Returns an empty list if the group does not exist.
   */
public slots:
  virtual void open();
  /*:F open
   *:D Opens this page, first updating all button texts based on the
       underlying ParamTree.
  */
  virtual void open(QString elt);
  /*:F open
   *:D This version opens a specific sub-page of a page that represents
       an array.
  */
  virtual void close();
  /*:F close
   *:D Closes this page.
   */
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
private:
  void representTabEnabled(QString id);
  /*:F representTabEnabled
   *:D Visually indicates which tabs have their 'enabled' button on.
   *:A id: local ID of button (e.g. 'channel*DO3')
   */
  void openLeaf(class Param *pp);
  /*:F openLeaf
   *:D Portion of open() that deals with leaf pages, i.e., menus and checklists.
   *:A pp: parameter this leaf page acts on.
   *:N This is where the button that represents the current value is selected,
       and where buttons that represent illegal values are greyed out.
   */
  void openNode();
  /*:F openNode
   *:D Portion of open() that deals with non-leaf pages, i.e., regular pages
       and tabbed pages.
   *:N This is where values are copied from the ParamTree to the page's buttons.
  */
  void reTree(class ParamTree *neworigtree=0);
  /*:F reTree
   *:D When a page tree with tabbed pages changes tabs, this makes the ParamTree
       of all pages below the changed tab point to the right data.
   *:A neworigtree: the ParamTree for this level, with arrays dereferenced at
       all higher levels, but not at this level.
  */
  void setEnabled(bool on, QString enabler);
  /*:F setEnabled
   *:D Enables or shades all buttons in this page and subpages.
   *:A enabler: if non-empty, any button with this ID is spared.
   */
  void updateEnabled();
  /*:F updateEnabled
   *:D Enables or shades buttons in this page depending on their "enable_if"
       status.
   *:N This also resets disabled values to their defaults.
  */
private:
  QMap<QString, class xmlButton *> buttons;
  /*:V buttons
   *:D All our buttons by ID. (ID is leaf, not full path).
   */
  QMap<QString, class QObject *> groups;
  /*:V groups
   *:D All our button groups by ID. (ID is leaf, not full path).
   */
  QMap<QString, QString> groupedButton;
  /*:V groupedButton
   *:D Map of buttonID to groupID for grouped buttons.
   */
  QString currentElement;
  /*:V currentElement
   *:D For pages that represent arrays (tabbed pages), this is the ID of the
       currently open tab.
  */
  QString caption;
  /*:V caption
   *:D Caption text to be placed above this page.
   */
  QMap<QString,QString> groupDefaults;
  /*:V groupDefaults
   *:D Map of default button names for each group that has one.
   */
  bool neverOpened;
  /*:V neverOpened
   *:D True if this page has never yet been opened.
   */
private slots:
  void addTriangle(QString id);
  void removeTriangle(QString id);
private:
  QPolygon triangle;
  QString triID;
  QColor triColor;
private:
  void setDefaultColors(QDomElement);
  Geom buttonGeometry(QDomElement);
  void buildChildren(Geom, QDomElement);
  void buildAutoItems();
  class AutoInfo {
  public:
    bool hasAutoItems;
    QDomElement doc;
    Geom geom;
  } autoInfo;
protected:
  virtual QString getCurrentElement() const;
};

#endif
