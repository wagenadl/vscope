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
