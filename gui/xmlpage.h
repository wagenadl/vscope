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

class xmlPage: public QFrame {
  Q_OBJECT;
  /*:C xmlPage
   *:D Represents a single page of buttons.
   */
signals:
  void opening(QString mypath, QWidget *me);
  /*:S opening
   *:D Emitted just before this page opens.
   *:A mypath: instantiated string path of this page
       me: pointer to this page as a widget
  */
  void opened(QString mypath, QWidget *me);
  /*:S opened
   *:D Emitted just after this page opens.
   *:A mypath: instantiated string path of this page
       me: pointer to this page as a widget
  */
  void closed(QString mypath, QWidget *me);
  /*:S closed
   *:D Emitted just after this page is closed.
   *:A mypath: instantiated string path of this page
       me: pointer to this page as a widget
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
  class xmlButton *findpButton(QStringList path);
  /*:F findpButton
   *:D Returns a pointer to a button given a path, or null if that button
       does not (currently) exist.
   *:N A path with arrays must be represented in the abstract, as in:
       "page.array.button", not "page.array:element.button".
  */
  class xmlPage *findpPage(QStringList path);
  /*:F findpPage
   *:D Returns a pointer to a page given a path, or null if that button
       does not (currently) exist.
   *:N A path with arrays must be represented in the abstract, as in:
       "page.array", not "page.array:element".
  */
  class xmlButton &findButton(QStringList path);
  /*:F findButton
   *:D Like findpButton, but returns a reference, and throws an exception
       if the path does not indicate a button.
  */
  class xmlPage &findPage(QStringList path);
  /*:F findpPage
   *:D Like findpPage, but returns a reference, and throws an exception
       if the path does not indicate a page.
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
  QString pathToLocal(QString path) const;
  /*:F pathToLocal
   *:D Given a global path (that must include this page as a head), removes
       the head part and returns only the part local to us.
   *:N For example, if we are called "stim/ephys" and the path is
       "stim/ephys/channel:A00/ntrains", this returns "channel:A00/ntrains".
       Note that this works even if there are any instantiated arrays
       upstream. For instance, if we are "stim/ephys/channel/ntrains", then
       "stim/ephys/channel:A01/ntrains/5" becomes "5", and if we are
       "stim/ephys/channel", then "stim/ephys/channel:A01" becomes ":A01".
   *:E If path does not reference something in our scope, an exception is
       thrown.
  */
  QString pathToGlobal(QString path) const;
  /*:F pathToGlobal
   *:D Given a local path, prepends our path to it to create a global path.
   *:N The local path may start with ":" if it specifies an array element.
   *:N We do not check whether path is actually a descendent of ours.
   */
  QString pathInstantiate(QString path) const;
  /*:F pathInstantiate
   *:D Given a global path, instantiates any uninstantiated arrays by their
       currently open element.
  */
  QString pathDeinstantiate(QString path) const;
  /*:F pathDeinstantiate
   *:D Given a global path, removes any ":elt" array indexing.
   */
public slots:
  void open();
  /*:F open
   *:D Opens this page, first updating all button texts based on the
       underlying ParamTree.
  */
  void open(QString elt);
  /*:F open
   *:D This version opens a specific sub-page of a page that represents
       an array.
  */
  void close();
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
  class xmlGui *master;
  /*:V master
   *:D xmlGui that created us or our root. Used for its ParamTree and its
       geometry info.
  */
  QMap<QString, class xmlButton *> buttons;
  /*:V buttons
   *:D All our buttons by ID. (ID is leaf, not full path).
   */
  QMap<QString, class xmlPage *> subPages;
  /*:V subPages
   *:D All our subpages by ID. (ID is leaf, not full path).
   */
  QMap<QString, class QObject *> groups;
  /*:V groups
   *:D All our button groups by ID. (ID is leaf, not full path).
   */
  QMap<QString, QString> groupedButton;
  /*:V groupedButton
   *:D Map of buttonID to groupID for grouped buttons.
   */
  class ParamTree *ptree;
  /*:V ptree
   *:D This points to the parameter tree of this page. For pages that represent
       arrays (tabbed pages), this either points to the full array (if
       currentElement is empty), or to a specific element.
  */
  class ParamTree *origptree;
  /*:V origptree
   *:D For pages that represent arrays this is the pointer to the tree of the
       full array.
  */
  QString currentElement;
  /*:V currentElement
   *:D For pages that represent arrays (tabbed pages), this is the ID of the
       currently open tab.
  */
  QString myPath;
  /*:V myPath
   *:D Full path to this page.
   */
  QString myTag;
  /*:V myTag
   *:D xml tag for this page, i.e., "page", "tabbedpage", "menu", or "checklist".
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
  void buildAutoItems();
  class AutoInfo {
  public:
    bool hasAutoItems;
    QDomElement doc;
    Geom geom;
  } autoInfo;
};

#endif
