// xmlgui.h

#ifndef XMLGUI_H

#define XMLGUI_H

#include <xml/param.h>
#include <QDomElement>
#include <QObject>
#include <gui/xmlbutton.h>

class xmlGui: public QObject {
  /*:C xmlGui
   *:D Represents the entire user interface of vsdscope except for
       the actual graphs and images.
       It reads an xml document which should define all the various buttons,
       menus, etc.
  */
  Q_OBJECT;
public:
  xmlGui(class QWidget *parent, class ParamTree *ptree, QDomElement doc);
  /*:F constructor
   *:D Constructs the user interface
   *:A parent: main widget of application.
    :  ptree: tree of configuration/acquisition parameters
    :  doc: the <vsdscopeConfig> element that defines the UI
   *:N It is assumed that relevant Enumerators have been loaded already.
   *:N The element must contain a <page> with id 'root' which defines the
       top level of the page tree, the page which is opened on program start.
   */
  virtual ~xmlGui();
  /*:F destructor
   */
  class xmlButton *findpButton(QString path);
  /*:F findpButton
   *:D Returns a pointer to a button given a path, or null if that button
       does not (currently) exist.
   *:N A path with arrays must be represented in the abstract, as in:
       "page.array.button", not "page.array:element.button".
  */
  class xmlPage *findpPage(QString path);
  /*:F findpPage
   *:D Returns a pointer to a page given a path, or null if that button
       does not (currently) exist.
   *:N A path with arrays must be represented in the abstract, as in:
       "page.array", not "page.array:element".
  */
  Param const *findpParam(QString path);
  /*:F findpParam
   *:D Returns a pointer to a parameter given a path, or null if no such
       parameter exists.
   *:N The path must reference arrays specifically, as in:
       "page.array:element.param"; parameters do not exist in the abstract.
   *:N This returns a const pointer; to set a parameter, use the setParam()
       family of functions.
  */
  class xmlButton &findButton(QString path);
  /*:F findButton
   *:D Like findpButton, but returns a reference, and throws an exception
       if the path does not indicate a button.
  */
  class xmlPage &findPage(QString path);
  /*:F findpPage
   *:D Like findpPage, but returns a reference, and throws an exception
       if the path does not indicate a page.
  */
  class xmlPage &rootPage() { return *root; }
  /*:F rootPage
   *:D Returns a reference to our root page.
   */
  Param const &findParam(QString path);
  /*:F findpParam
   *:D Like findpParam, but returns a reference, and throws an exception
       if the path does not indicate a parameter.
  */
  class guiGeom const &geom() const { return *geom_; }
signals:
  void buttonClicked(QString path, QString text);
  /*:F buttonClicked
   *:D Emitted whenever an action button is clicked.
   *:A path: the full path to the button, not including array instantiations.
             (Use pathInstantiate() to get the current instantiation.)
       text: the current text displayed in the button.
   */
  void buttonSelected(QString path, QString text);
  /*:F buttonSelected
   *:D Emitted whenever a button is selected.
   *:N Note that selections within menus automatically trigger parameter
       changes, and menus and subpages are opened and closed automatically
       as well; this is all handled internally. Signals are not emitted
       for menu items.
   */
  void buttonDeselected(QString path, QString text);
  /*:F buttonDeselected
   *:D Emitted whenever a button is deselected.
   */
  void paramChanged(QString path, QString newval);
  /*:F paramChanged
   *:D Emitted whenever a parameter changes value, either because of direct
       user action, or because of a call to setParam.
   *:N Since a requested parameter change can affect other parameters,
       e.g., if both a 'frequency' and a 'period' menu is offered for the
       same physical parameter, more than one signal may be emitted following
       a single call to setParam.
  */
  void customValueChanged(QString path, int customid, QString newval);
  /*:F customValueChanged
   *:D Emitted whenever a custom value is changed.
   */
  void pageOpening(QString path, QWidget *pg);
  /*:F pageOpening
   *:D Emitted whenever a page is about to open.
   *:A path: instantiated path to the page.
    :  pg: pointer to the page as a widget.
  */
  void pageOpened(QString path, QWidget *pg);
  /*:F pageOpening
   *:D Emitted whenever a page has just been opened.
   *:A path: instantiated path to the page.
    :  pg: pointer to the page as a widget.
  */
  void pageClosed(QString path, QWidget *pg);
  /*:F pageClosed
   *:D Emitted whenever a page has just been closed.
   *:A path: instantiated path to the page.
    :  pg: pointer to the page as a widget.
  */
public:
  virtual void setParam(QString path, QString newval);
  /*:F setParam
   *:D Assigns a new value to a parameter, taking care of side effects.
   *:N Throws an exception if 'path' does not reference a parameter or
       if the value is invalid (either globally, or in the context of other
       current parameter values).
  */
  virtual void setCustom(QString path, int cno, QString newval);
  /*:F setParam
   *:D Assigns a new value to a a custom value for parameter.
   *:N Throws an exception if 'path' with 'cno' does not reference a
       parameter. Value checking is not performed.
  */
  virtual bool canSetParam(QString path, QString newval);
  /*:F canSetParam
   *:D Returns true if 'newval' is a valid new value for the parameter
       at path (in the context of other current parameter values).
   *:N Throws an exception if 'path' does not reference a parameter.
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
   *:D Opens the root page of the gui.
   */
  void close();
  /*:F close
   *:D Closes the root page of the gui.
   */
protected:
  friend class vsdscopeGui;
  Param *accesspParam(QString path);
  Param &accessParam(QString path);
  /*:F accesspParam and accessParam
   *:D These are non-const versions of findParam only for use by our
       descendents.
   *:N (They are needed for setParam.)
  */
private:
  class xmlPage *root;
  class ParamTree *ptree;
  class guiGeom *geom_;
};

#endif
