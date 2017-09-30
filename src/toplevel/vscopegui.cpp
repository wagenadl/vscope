// toplevel/vscopegui.cpp - This file is part of VScope.
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

// vscopegui.cpp

#include <toplevel/vscopegui.h>
#include <xml/paramtree.h>
#include <base/exception.h>

vscopeGui::vscopeGui(class QWidget *parent, class ParamTree *ptree, QDomElement doc):
  guiRoot(parent,ptree,doc) {
}

vscopeGui::~vscopeGui() {
}

void vscopeGui::setParam(QString path, QString newval) {
  guiRoot::setParam(path,newval);

  // take care of side effects here:
  // ...
}

bool vscopeGui::canSetParam(QString path, QString newval) {
  if (!guiRoot::canSetParam(path,newval))
    return false;

  // check if this value is OK in combination with others:
  // ...
  return true;
}
