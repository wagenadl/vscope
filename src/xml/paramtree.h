// xml/paramtree.h - This file is part of VScope.
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

// paramtree.h

#ifndef PARAMTREE_H

#define PARAMTREE_H

#include <xml/param.h>

#include <QDomElement>

class ParamTree {
  /*:C ParamTree
   *:D A ParamTree represents a tree of parameters as defined by a <params>
       element.
  */
public:
  ParamTree(ParamTree const &other); // deep copy
  ParamTree(QDomElement doc);
  /*:F constructor
   *:D This builds the tree.
   *:A doc: a <params>, <category>, <array>, or <param> element,
            or a <vsdscopeDefs> element containing a <params> element.
   *:N If the tree contains any arrays, this requires that the relevant
       Enumerators have already been setup.
   */
  void read(QDomElement doc);
  /*:F read
   *:D This reads all values in doc, into the tree.
       Values that are not defined in doc are unaffected. Values defined
       in doc but not in our tree cause a warning.
   *:A doc: a <settings>, < element or an element containing a <settings>
       element.
  */
  void write(QDomElement doc) const;
  /*:F write
   *:D This writes all values from the tree into doc.
   *:A doc: a <settings> element or an element containing a <settings> element.
       New child nodes are created inside doc as needed.
   *:N If doc contains children that are not matched by our own children, those
       are not affected.
   *:N The "savable" flag of this ParamTree itself is irrelevant, but the
       "savable" flag of children determines whether those children are saved
       or not.
  */
  void reset();
  /*:F reset
   *:D This reloads default values from the <params> element passed to the
       constructor.
  */
  int report(int level=0, int maxelts=1000000000) const;
  /*:F report
   *:D This writes the contents of the tree to stdout.
   */
  Param const *leafp() const;
  Param *leafp();
  /*:F leafp
   *:D Returns a pointer to the parameter stored at this node, if any.
   *:N Returns null if there is no parameter stored at this node.
   */
  Param const &leaf() const;
  Param &leaf();
  /*:F leaf
   *:D Returns a reference to the parameter stored at this node, if any.
   *:N This throws an exception if there is no parameter stored at this node.
   */
  class ParamTree const *childp(QString name) const;
  class ParamTree *childp(QString name);
  /*:F childp
   *:D Returns a pointer to an immediate descendent of this tree.
   *:A name: the name of the descendent. 
   *:R Returns null if not found.
   *:N This will not search subtrees, so 'name' cannot be a multi-level path.
   */
  class ParamTree const &child(QString name) const;
  class ParamTree &child(QString name);
  /*:F child
   *:D Returns a reference to an immediate descendent of this tree.
   *:A name: the name of the descendent. 
   *:N This throws an exception if there is no parameter stored at this node.
   *:N This will not search subtrees, so 'name' cannot be a multi-level path.
   */
  class ParamTree const *treep(QString name) const;
  class ParamTree *treep(QString name);
  /*:F treep
   *:D Returns a pointer to any descendent of this tree.
   *:A name: the name of the descendent. 
   *:R Returns null if not found.
   *:N This *will* search subtrees; 'name' can be a multi-level path.
   */
  class ParamTree const &tree(QString name) const;
  class ParamTree &tree(QString name);
  /*:F tree
   *:D Returns a reference to any descendent of this tree.
   *:A name: the name of the descendent. 
   *:N This *will* search subtrees, so 'name' can be a multi-level path.
   */
  Param const *findp(QString path) const;
  Param *findp(QString path);
  /*:F findp
   *:D Returns a pointer to a param somewhere down in this tree.
   *:A path: slash- and colon-separated path of the item.
   *:R Pointer to the item, or null if not found.
   */  
  Param const &find(QString path) const;
  Param &find(QString path);
  /*:F find
   *:D Returns a reference to a param somewhere down in this tree.
   *:A path: slash- and colon-separated path of the item.
   *:N Throws an exception if not found.
   */
  bool enabled(QString path) const;
  /*:F enabled
   *:D Returns true if the path is enabled. A path is considered enabled
       unless PATH/enable is false or any "enable" parameter above PATH
       is false. For instance "stimEphys/channel:DO3" is enabled if
       stimEphys/enable is true and stimEphys/channel:DO3/enable is true.
   *:N This function does not check if PATH is legitimate and returns true
       for paths that have no enable flag or that do not exist at all.
  */
  bool isSavable() const;
  /*:F isSavable
   *:D Returns whether or not this tree should be saved according to the
       value of the "save" attribute from the XML that first defined it.
   *:N Tree defined using the non-xml constructor default to savable
       as do trees read from XML without a "save" attribute.
  */
  void setSavable(bool);
  /*:F setSavable
   *:D Define whether or not this parameter should be saved.
   */
  bool isArray() const;
  QList<QString> childIDs() const;
protected:
  ParamTree(QDomElement doc, QString elt);
  /*:F constructor
   *:D This builds one element of an array.
   */
  void construct();
  void buildEnablers();
  /*:F buildEnablers
   *:D This creates the cond-attribute links between elements.
   */
  void decideSavable(QDomElement e);
protected:
  QDomElement base;
  QString arrayElement;
  Param *leaf_;
  //  ParamTree *parent;
  QMap<QString, ParamTree *> children;
  bool savable;
  QMap<QString, QString> condmap;
public:
  QString dbgPath;
  void setDbgPath(QString path);
private:
  ParamTree &operator=(ParamTree const &); // not defined
};

#endif
