// param.h

#ifndef PARAM_H

#define PARAM_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QRect>
#include <QDomElement>

class Param {
  /*:C Param
   *:D Param represents the type and value of one parameter (as declared
       by a <param> xml element inside a <vsdscopeParameters> block).
       Parameters can have the following types:

         int - An integer.
         double - A double floating point number.
         bool - True or false. 
                Valid string representations of true are "1", "true", "on",
                and "yes". Converted back to string, true is always
                represented as "1".
                Valid string representations of false are "0", "false",
                "off", and "no". Converted back to string, false is always
                represented as "0".
         time - A time, internally stored in milliseconds. Valid string
                representations are: a plain number (which is interpreted as
                milliseconds), or a number followed by one of the units "us",
                "ms", "s", "min", or "h". Converted back to string, a unit
                is automatically chosen to make the number look reasonable.
         freq - A frequency, internally stored in Hertz. Valid string
                representations are: a plain number (which is interpreted as
                Hertz), or a number followed by one of the units "Hz",
                or "kHz". Converted back to string, a unit
                is automatically chosen to make the number look reasonable.
	 voltage - A voltage, internally stored in millivolts. Valid string
	        representations are: a plain number (which is interpreted as
                mV), or a number followed by one of the units "uV", "mV", or
                "V". Converted to double, the value is returned in mV.
		Converted back to string, a unit is automatically chosen
		to make the number look reasonable.
	 current - A current, internally stored in nanoamps. Valid string
	        representations are: a plain number (which is interpreted as
                nA), or a number followed by one of the units "pA", "nA", or
                "uA" or "mA". Converted to double, the value is returned
		in nA. Converted to string, a unit is automatically chosen
		to make the number look reasonable.
         geometry - A pixel geometry, internally stored as a QRect. The only
                valid string representation is "WxH+XL+YT".
         enum - A single enumeration value. The Enumerator that this value
                belongs to is contained in the object. May be assigned as an
                int, if that integer is one of the Enumerator's constants, 
                or as a string (using one of the Enumerator's names for those
                constants). The string representation never includes the
                name of the enumerator itself.
         set -  A set of enumeration values from the same Enumerator. May
                be assigned as a string consisting of space-separated enum
                constant names.
	 string - A string.
	 percentage - A percentage stored as a double between 0 and 100.
	        Percentages can be read and written as floating point
		values, but are represented with "%" added.
  */
public:
  Param(QString type="", QString enumname="");
  /*:F constructor
   *:D This constructor creates a Param of a given type. If the type is "enum"
       or "set", the name of the Enumerator must also be specified. As a
       special case, when called without a type, constructs a dummy Param
       that is invalid until the type is specified through newType().
   *:N None of the constructors set the "enabled" mechanism. This is
       done externally through setEnabler().
  */
  Param(QDomElement defn);
  /*:F constructor
   *:D This constructor creates a Param based on a <param> xml element.
   */
  Param(QDomElement doc, QString path);
  /*:F constructor
   *:D Same as above, except that it first finds the <param> element inside 
       a larger document based on the given path. The path consists of
       dot-separated IDs of categories and arrays.
       For instance, "stimEphys.channels.ntrains" would be a valid path.
       Note that no index is specified into arrays.
  */
  Param(Param const &src);
  /*:F constructor
   *:D This creates a direct copy of another Param.
   *:N The "value" of the Param is deep-copied, but the "min" and "max"
       parameters are shallow-copied.
  */
  ~Param();
  /*:F destructor
   *:N There is a known memory leak in Param: the "min" and "max" parameters
       are never destructed, since I am too lazy to implement a reference
       counting scheme for them. (They are shallow-copied by operator=().)
  */
  void newDefn(QDomElement defn);
  /*:F newDefn
   *:D Typically used shortly after construction, this rebuilds this parameter
       based on the given xml element. See the constructors for more info.
  */
  void newType(QString type, QString enumname="");
  /*:F newType
   *:D Typically used shortly after construction, this resets the type and
       optionally Enumerator name of the value represented here.
   *:N This invalidates the current value, so you must call one of the set()
       or read() functions soon after.
  */
  Param &operator=(Param const &src);
  /*:F operator=
   *:D This simply assigns the value from another parameter to this one.
   *:N An exception is generated unless the types (and enumerators) match.
   */
  bool operator==(Param const &oth) const;
  /*:F operator==
   *:D Compares to parameters.
   *:R Returns false if they are not of the same type.
   */
  QString toString() const;
  /*:F toString
   *:D Returns a string representation of this value. All values can be 
       represented as strings.
  */
  int toInt() const;
  /*:F toInt
   *:D Returns an integer representation of this value. This may not be
       meaningful for some types.
  */
  double toDouble() const;
  /*:F toDouble
   *:D Returns a floating point representation of this value. This may not be
       meaningful for some types.
  */
  QRect toRect() const;
  /*:F toRect
   *:D Returns a rectangle representation of this value. Only valid for
       type "geometry".
  */
  bool toBool() const;
  /*:F toBool
   *:D Returns a boolean representation of this value. This may not be
       meaningful for some types.
  */
  QSet<QString> toStrings() const;
  /*:F toStrings
   *:N Only type "set" can be represented as a set of strings.
   */
  void read(QDomElement v);
  /*:F read
   *:D Reads the value of a <pval> xml element into this parameter.
   *:N Throws an exception if the <pval> contains an inappropriate
       representation.
  */
  bool read(QDomElement doc, QString path);
  /*:F read
   *:D As above, but first looks up the <pval> in a larger document
       using the named path. If there is no such <pval>, nothing is done.
       For instance, "stimEphys.channels:DO1.ntrains" would be a valid path.
       Note array indices *must* be specified. They follow the array name
       after a colon.
   *:R True iff the path yielded a match.
  */
  void write(QDomElement v);
  /*:F write
   *:D Writes this value to a <pval> xml element.
   */
  void write(QDomElement doc, QString path);
  /*:F write
   *:D As above, but first looks up the <pval> in a larger document
       using the named path. If there is no such <pval> in the document,
       it is created. As for read(), array indices must be specified.
  */
  void set(QString s);
  /*:F set 
   *:D Sets this parameter based on a string representation.
   *:N Throws an exception if conversion failed.
   *:N For "set" parameters, you can set/clear single bits by passing
       "+ELEM" or "-ELEM" to set().
   */
  void setInt(int i); 
  /*:F setInt
   *:D Sets this parameter based on an integer representation.
   *:N Throws an exception if this type cannot be represented as an integer,
       or if the particular integer is not a valid value.
  */
  void setDouble(double f);
  /*:F setDouble
   *:D Sets this parameter based on a floating point representation.
   *:N Throws an exception if this type cannot be represented as a double.
   */
  void setBool(bool b); 
  /*:F setBool
   *:D Sets this parameter based on a bool representation.
   *:N Throws an exception if this type cannot be represented as a bool.
  */
  void setRect(QRect const &r);
  /*:F setRect
   *:D Sets this parameter based on a QRect representation.
   *:N Only type "geometry" can be represented as a QRect.
   */
  void setStrings(QSet<QString> const &ss);
  /*:F setStrings
   *:N Only type "set" can be set this way.
   */
  void report() const;
  /*:F report
   *:D Reports type and value to stdout.
   */
  QString getType() const { return valueType; }
  /*:F getType
   *:D Returns the type of this parameter.
   *:N See class comments for type names.
   */
  class Enumerator const *getEnum() const { return enumerator; }
  /*:F getEnum
   *:D Returns a pointer to the enumerator for this parameter.
   *:N Only "enums" and "sets" have enumerators. Others return 0.
   */
  bool isEnabled() const;
  /*:F isEnabled
   *:D Returns true unless this parameter is part of a ParamTree and
       had a "cond" attribute when it was first read. In that case, 
       it returns true iff the condition is met.
  */
  bool isImmune() const;
  /*:F isImmune
   *:D True if this param can be set even on a nominally read-only tree */
  void restore();
  /*:F restore
   *:D Restore default value. That is: the value of the "default"
       attribute read when the param was first defined.
   *:N Note that the read() function can no longer affect the default.
   */
  void setEnabler(Param *cond, QString enable_if);
  /*:F setEnabler
   *:D To be called by ParamTree only, to specify which other Param determines
       whether this Param should be enabled.
   *:A cond: pointer to Param we depend on, or zero if we are not dependent
             on any one.
    :  enable_if: space-separated list of values for cond that enable us. We
                  should be disabled (and restored to default) in all other
                  cases. 
                  Special case: if cond=0 and enable_if="*never", we will
                  never be enabled.
   *:N Note that this mechanism is mainly useful if cond points to an enum.
   */
  void setEnabler(Param *cond);
private:
  void rangeCheck(QVariant const &v);
  /*:F rangeCheck
   *:D Checks that v is in the range represented by min and max, if
       this param is of a checkable type.
       Range checking is only supported for types that can be meaningfully
       represented as doubles. In particular, not for enums and sets.
       If the check fails, an exception is generated. Otherwise, our
       value is set to v.
  */
private:
  QVariant value;
  QString valueType;
  class Enumerator *enumerator;
  class Param *min, *max; 
  /*:M min, max
   *:D These represent the "min" and "max" attributes of a <param> element.
  */
  class Param *cond; // if null, param is always enabled unless enable_if="*never".
  QStringList enable_if;
  QVariant deflt;
  bool immune;
public:
  QString dbgPath; // my full path, for debugging only
};

#endif
