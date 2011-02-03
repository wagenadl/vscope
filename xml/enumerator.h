// enumerator.h

#ifndef ENUMERATOR_H

#define ENUMERATOR_H

#include <QDomElement>
#include <QMap>
#include <QString>

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
  Enumerator();
  /*:F constructor
   *:D This constructor creates a null enumerator, without any constants. 
   *:N This is not very useful.
   */
  ~Enumerator();
  Enumerator(QDomElement def);
  /*:F constructor
   *:D This reads values from an <enum> xml element
   *:A def: should be of type <enum>; this is not explicitly verified.
   */
  void add(QString s, int n);
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
  int getLargestValue() const { return largestValue; }
  /*:F getLargestValue
   *:D Returns the largest numerical value included in this set.
   *:N If the set is empty, returns -1000.
   */
  int getSmallestValue() const { return smallestValue; }
  /*:F getSmallestValue
   *:D Returns the smallest numerical value included in this set.
   *:N If the set is empty, returns 1000.
   */
  bool isEmpty() const { return values.empty(); }
  QStringList getAllTags() const;
private:
  QMap<QString,int> values;
  QMap<int,QString> tags;
  int smallestValue, largestValue;
  QString myname;
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
