// acq/ccdmaster.h - This file is part of VScope.
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

// ccdmaster.h

#ifndef CCDMASTER_H
#define CCDMASTER_H

#include <xml/paramtree.h>

ParamTree const *camTreep(ParamTree const *ptree, QString camid);
/*:F camTreep
 *:D Given the root ParamTree, looks in "acqCCD/camera:*" for a camera
     named CAMID and returns a pointer to the corresponding ParamTree.
     This function recurses through "master" references.
 *:N Returns zero if the camera is not found in the tree.
 */

ParamTree const &camTree(ParamTree const *ptree, QString camid);
/*:F camTreep
 *:D Given the root ParamTree, looks in "acqCCD/camera:*" for a camera
     named CAMID and returns a pointer to the corresponding ParamTree.
     This function recurses through "master" references.
 *:N Throws an exception if the camera is not found in the tree.
 */

QString immediateCCDMaster(ParamTree const *ptree, QString camid);
  /* Returns the master of camera ID, or "" if self. */

QString ultimateCCDMaster(ParamTree const *ptree, QString camid);
  /* Returns the (master of the master of the) master of camera ID,
     or "" if self, or "LOOP" if loop detected. */

#endif
