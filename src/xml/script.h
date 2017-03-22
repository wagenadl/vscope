// xml/script.h - This file is part of VScope.
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

// script.h

#ifndef SCRIPT_H

#define SCRIPT_H

#include <xml/paramtree.h>
#include <QString>

class Script {
  /*:C Script
   *:D A Script is a text file with lines of the form
       KEYWORD ARGUMENT [ARGUMENT ...]
       Any text after "#" is ignored.
       Syntax:
         set PARNAME PARVALUE - Sets a parameter
	 trial - Runs a trial
	 snap - Takes a single frame
	 ival TIME - Sets (start-to-start) time between trials. Note that
	             this is applied *after* the next trial.
	 loadsettings FILENAME - Loads a settings file
	 loop [LINENO] - Loops starting at the beginning or at the given
	                 line (counts from 1).
       Example:
         loadsettings vidblock  # Note: no path, no ".xml".
	 ival 60 s
	 set stimVideo/parB 90
	 trial # happens right away
	 set stimVideo/parB 270
	 trial # happens 60 s later
	 # etcetera
       If a trial takes longer than the interval, the next trial happens
       very quickly after its end: a watchdog timer checks for trial's end
       every second.
   */
public:
  enum Keyword {
    KW_SET,
    KW_TRIAL,
    KW_SNAP,
    KW_IVAL,
    KW_LOADSETTINGS,
    KW_LOOP,
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
