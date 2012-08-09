// enumerator.h

#ifndef ENUMERATOR_H

#define ENUMERATOR_H

#include <QDomElement>
#include <QMap>
#include <QString>
#include <QStringList>

class Enumerator {
  /*:C Enumerator
   *:D This class holds the values of a set of related enumeration constants.
       The constants are normally read from an XML element of the form:

         <enum id="NAME">
           <item tag="NAME1" value="INT1"/>
           <item tag="NAME2" value="INT2"/>
           ...
         </enum>
  */
public:
  Enumerator(QString name="-");
  /*:F constructor
   *:D This constructor creates a null enumerator, without any constants. 
   */
  ~Enumerator();
  Enumerator(QDomElement def);
  /*:F constructor
   *:D This reads values from an <enum> xml element
   *:A def: should be of type <enum>; this is not explicitly verified.
   *:N The <enum> may define the name of this enumerator through its "id" tag.
       It should define values through <item tag="KEY" value="VAL"/> elements.
   */
  void add(QString s);
  void add(QString s, int n);
  void add(QDomElement def);
  /*:F add
   *:D Add a new value or a lot of values from an <enum> xml.
   *:N DEF may be an <enum> or <item> element.
   *:N The first form automatically assigns a new value larger than any
       previous value.
   */
  int lookup(QString s) const;
  int lookup(QString s, int dflt) const;
  /*:F lookup
   *:D Looks up the numerical value of an enumeration constant by name.
   *:A s: Name of constant (i.e. one of "NAME1", "NAME2", ...)
   *:R The corresponding numerical value (i.e. INT1, INT2, ...)
   *:N If dflt is not given, an exception is thrown if the constant
       is not part of the enumeration.
   */
  bool has(QString s) const;
  /*:F has(QString)
   *:D Returns true iff s names an enumeration value in this set.
   */
  bool has(int n) const;
  /*:F has(int)
   *:D Returns true iff n is the value of one of the constants in this set.
   */
  QString reverseLookup(int n) const;
  /*:F reverseLookup
   *:D Returns the name corresponding to the given numerical value.
   *:A n: numerical value to look up (i.e. one of INT1, INT2, ...).
   *:R Corresponding name (i.e. one of "NAME1", "NAME2", ...).
   *:N Throws an exception if the number is not part of the set.
   */
  QString getName() const { return myname; }
  /*:F getName
   *:D Returns the name of this enumeration, i.e., the value of its "id" tag.
   */
  int getLargestValue() const { return largestVal; }
  /*:F getLargestValue
   *:D Returns the largest numerical value included in this set.
   *:N If the set is empty, return value is undefined.
   */
  int getSmallestValue() const { return smallestVal; }
  /*:F getSmallestValue
   *:D Returns the smallest numerical value included in this set.
   *:N If the set is empty, return value is undefined.
   */
  bool isEmpty() const { return tag2val.empty(); }
  /*:F isEmpty
   *:D Returns true if there are no actual definitions in this enum
   */
  QStringList getAllTags() const;
  QStringList getNonnegativeTags() const;
  /*:F getAllTags, getNonnegativeTags
   *:D Returns a list of all tags, or all nonnegative tags, in the order
       in which they were introduced.
   */
  void remove(QString id);
  /*:F remove
   *:D Remove a single tag. It is not an error if the tag didn't exist
       in the first place.
  */
  void reset();
  /*:F reset
   *:D Drops all tags
   */
private:
  QMap<QString,int> tag2val;
  QMap<int,QString> val2tag;
  int smallestVal, largestVal;
  QString myname;
  QStringList orderedTags;
public:
  static Enumerator *find(QString e);
  /*:F find
   *:D This static function finds an enumerator by name.
   *:N Throws an exception if the enumerator does not exist.
    :  (This is based on each Enumerator registering itself in a global
       map upon creation.)
  */
  static void readAll(QDomElement doc);
  /*:F readAll
   *:D This static function reads all enumerator definitions from an xml
       element which must be of type <enums> or <vscscopeDefs> containing
       <enums>.
  */
  static QMap<QString,Enumerator *> enums;
};

#endif
