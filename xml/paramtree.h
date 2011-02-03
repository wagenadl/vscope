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
  ParamTree(ParamTree &other);
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
   *:A doc: a <settings>, < element or an element containing a <settings> element.
  */
  void write(QDomElement doc) const;
  /*:F write
   *:D This writes all values from the tree into doc.
   *:A doc: a <settings> element or an element containing a <settings> element.
       New child nodes are created inside doc as needed.
   *:N If doc contains children that are not matched by our own children, those
       are not affected.
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
protected:
  QDomElement base;
  QString arrayElement;
  Param *leaf_;
  //  ParamTree *parent;
  QMap<QString, ParamTree *> children;
#if PARAM_DBG
public:
  QString dbgPath;
  void setDbgPath(QString path);
#endif
};

#endif
