// toplevel/vscopegui.h - This file is part of VScope.
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

// vscopegui.h

#ifndef VSCOPEGUI_H

#define VSCOPEGUI_H

#include <gui/guiroot.h>

class vscopeGui: public guiRoot {
  /*:C vscopeGui
   *:D vscope-specific version of guiRoot.
       This class is quite specific to vscope; it knows, for instance,
       which parameters are dependent on each other and what are the
       acceptable combinations.
  */
  Q_OBJECT;
public:
  vscopeGui(class QWidget *parent, class ParamTree *ptree, QDomElement doc);
  virtual ~vscopeGui();
public:
  virtual void setParam(QString path, QString newval);
  /*:F setParam
   *:D Assigns a new value to a parameter, taking care of side effects.
       Several versions exist, to set parameters of various kinds.
   *:N Throws an exception if 'path' does not reference a parameter or
       if the value is invalid (either globally, or in the context of other
       current parameter values).
  */
  virtual bool canSetParam(QString path, QString newval);
  /*:F canSetParam
   *:D Returns true if 'newval' is a valid new value for the parameter
       at path (in the context of other current parameter values).
   *:N Throws an exception if 'path' does not reference a parameter.
   */
};

#endif
