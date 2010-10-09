// script.h

#ifndef SCRIPT_H

#define SCRIPT_H

#include <xml/paramtree.h>
#include <QString>

class Script {
public:
  enum Keyword {
    KW_SET,
    KW_TRIAL,
    KW_SNAP,
    KW_IVAL,
    KW_LOADSETTINGS,
  };
  class Command {
  public:
    Keyword kwd;
    QString arg1;
    QString arg2;
    double argv;
  };
public:
  Script();
  bool setText(QString const &);
  /*:F setText
   *:D Replaces the current text by new text.
   *:R Returns true if the text parses OK. This does NOT test that
       all parameter values are acceptable. See "ok" for that.
  */
  bool ok(ParamTree /*const */&ptree);
  /*:F ok
   *:D Checks whether all SET commands are acceptable, and whether all
       LOADSETTINGS commands refer to existing and loadable files.
   *:R Returns true if everything is OK.
  */
  QString firstError() const;
  /*:F firstError
   *:D If setText() or ok() returns false, this function returns the text
       of the first error.
   */
  QList<Command> const &contents() const;
private:
  QList<Command> lines;
  QString err;
};

#endif
