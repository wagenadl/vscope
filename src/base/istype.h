// istype.h

#ifndef ISTYPE_H

#define ISTYPE_H

template <class DstTyp, class SrcTyp>
bool isType(const SrcTyp *ptr) {
  return dynamic_cast<const DstTyp *>(ptr) != 0;
}
/*:F isType
 *:D isType<Y>(p) returns true iff p is a pointer to type Y.
 *:N This only works if the stated type of p is a parent of Y with at least
     one virtual function (i.e., if dynamic_cast<> can do the job).
*/

#endif
