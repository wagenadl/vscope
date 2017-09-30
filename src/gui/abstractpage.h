// gui/abstractpage.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// abstractpage.h
/* Base for xmlpage.h that knows about the hierarchy but not about visuals. */

#ifndef ABSTRACTPAGE_H
#define ABSTRACTPAGE_H

#include <QFrame>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QDomElement>
#include <gfx/button.h>

class AbstractPage: public QFrame {
  Q_OBJECT;
  /*:C AbstractPage
   *:D Base class for xmlPage that knows about the hierarchy but not
       about visuals.
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
  AbstractPage(class QWidget *parent,
	       class ParamTree *ptree,
	       QString id,
	       class guiRoot *master,
	       class QRect const &geom);
  virtual ~AbstractPage();

  AbstractPage *findpPage(QStringList path);
  /*:F findpPage
   *:D Returns a pointer to a page given a path, or null if that button
       does not (currently) exist.
   *:N A path with arrays must be represented in the abstract, as in:
       "page.array", not "page.array:element".
  */
  AbstractPage &findPage(QStringList path);
  /*:F findpPage
   *:D Like findpPage, but returns a reference, and throws an exception
       if the path does not indicate a page.
  */
 class guiButton *findpButton(QStringList path);
  /*:F findpButton
   *:D Returns a pointer to a button given a path, or null if that button
       does not (currently) exist.
   *:N A path with arrays must be represented in the abstract, as in:
       "page.array.button", not "page.array:element.button".
  */
 class guiButton &findButton(QStringList path);
  /*:F findButton
   *:D Like findpButton, but returns a reference, and throws an exception
       if the path does not indicate a button.
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
  virtual guiButton const *buttonp(QString id) const=0;
  virtual guiButton *buttonp(QString id)=0;
  /*:F buttonp
   *:D Get pointer to button that is an immediate child of our page.
   */
  virtual AbstractPage const *subpagep(QString id) const;
  virtual AbstractPage *subpagep(QString id);
  /*:F subpagep
   *:D Get pointer to page that is an immediate child of our page.
   */
  guiRoot const *masterp() const { return master; }
  QString id() const { return myId; }
  QString path() const { return myPath; }
  bool isReadOnly() const { return readonly; }
public slots:
  virtual void open()=0;
  virtual void close()=0;
  virtual void setReadOnly(bool);
  virtual void reTree(class ParamTree *neworigtree=0);
  /*:F reTree
   *:D When a page tree with tabbed pages changes tabs, this makes the ParamTree
       of all pages below the changed tab point to the right data.
   *:A neworigtree: the ParamTree for this level, with arrays dereferenced at
       all higher levels, but not at this level.
  */
protected:
  class guiRoot *master;
  /*:V master
   *:D guiRoot that created us or our root. Used for its ParamTree and its
       geometry info.
  */
  QMap<QString, AbstractPage *> subPages;
  /*:V subPages
   *:D All our subpages by ID. (ID is leaf, not full path).
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
  QString myPath;
  /*:V myPath
   *:D Full path to this page.
   */
  QString myId;
  /*:V myId
   *:D My leaf ID
   */
  bool readonly;
public:
  virtual QString getCurrentElement() const=0;
protected:
  virtual void connectToMaster(QDomElement)=0;
  virtual void connectToParent(QDomElement)=0;
  virtual void prepForOpening() {}
};  
  
#endif
