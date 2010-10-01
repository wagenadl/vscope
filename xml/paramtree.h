// paramtree.h

#ifndef PARAMTREE_H

#define PARAMTREE_H

#include <xml/param.h>

#include <QDomElement>

#define PTREE_PATHSEP '/'

class ParamTree {
  /*:C ParamTree
   *:D A ParamTree represents a tree of parameters as defined by a <params>
       element.
  */
public:
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
  void write(QDomElement doc);
  /*:F write
   *:D This writes all values from the tree into doc.
   *:A doc: a <settings> element or an element containing a <settings> element.
       New child nodes are created inside doc as needed.
   *:N If doc contains children that are not matched by our own children, those
       are not affected.
   *:N This should be labeled const, but I don't want to create const versions of
       each of the find-related functions below.
  */
  void reset();
  /*:F reset
   *:D This reloads default values from the <params> element passed to the
       constructor.
  */
  int report(int level=0, int maxelts=1000000000);
  /*:F report
   *:D This writes the contents of the tree to stdout.
   */
  Param *leafp();
  /*:F leafp
   *:D Returns a pointer to the parameter stored at this node, if any.
   *:N Returns null if there is no parameter stored at this node.
   */
  Param &leaf();
  /*:F leaf
   *:D Returns a reference to the parameter stored at this node, if any.
   *:N This throws an exception if there is no parameter stored at this node.
   */
  class ParamTree *childp(QString name);
  /*:F childp
   *:D Returns a pointer to an immediate descendent of this tree.
   *:A name: the name of the descendent. 
   *:R Returns null if not found.
   *:N This will not search subtrees, so 'name' cannot be a multi-level path.
   */
  class ParamTree &child(QString name);
  /*:F child
   *:D Returns a reference to an immediate descendent of this tree.
   *:A name: the name of the descendent. 
   *:N This throws an exception if there is no parameter stored at this node.
   *:N This will not search subtrees, so 'name' cannot be a multi-level path.
   */
  Param *findp(QString path);
  /*:F findp
   *:D Returns a pointer to a param somewhere down in this tree.
   *:A path: slash- and colon-separated path of the item.
   *:R Pointer to the item, or null if not found.
   */  
  Param &find(QString path);
  /*:F find
   *:D Returns a reference to a param somewhere down in this tree.
   *:A path: slash- and colon-separated path of the item.
   *:N Throws an exception if not found.
   */
  // Param *findpCustom(QString path, int custom);
  /*:F findpCustom
   *:D Returns a pointer to a custom value for a param somewhere down
       in this tree.
   *:A path: slash- and colon-separated path of the item itself, not the
             custom value.
    :  custom: number identifying the custom value.
   *:N This creates the custom param if it doesn't already exist, but
       null is returned if 'path' doesn't refer to a Param.
   *:N Custom parameters are named NAME-NUMBER, where NAME is the name
       of the Param being customized, and NUMBER is the custom number.
  */
  // Param &findCustom(QString path, int custom);
  /*:F findCustom
   *:D Returns a reference to a custom value for a param somewhere down
       in this tree.
   *:A path: slash- and colon-separated path of the item itself, not the
             custom value.
    :  custom: number identifying the custom value.
   *:N This creates the custom param if it doesn't already exist.
   *:N An exception is thrown if path does not refer to a Param.
   *:N Custom parameters are named NAME-NUMBER, where NAME is the name
       of the Param being customized, and NUMBER is the custom number.
  */
  //uint64_t checksum();
  /*:F checksum
   *:D Calculates a number that is (hopefully) unique to the current set
       of values.
   *:N I am not a specialist in this business, so I am far from sure the
       hashing is a good one.
  */
protected:
  ParamTree(QDomElement doc, QString elt);
  /*:F constructor
   *:D This builds one element of an array.
   */
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
