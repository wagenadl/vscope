// memalloc.h

#ifndef MEMALLOC_H

#define MEMALLOC_H

#include <base/exception.h>

class MemExc: public Exception {
  /*:C MemExc
   *:D A descendent of Exception that reports memory allocation failures.
   */
public:
  MemExc(QString issuer="");
  /*:F daqMemExc(QString issuer="")
   *:A QString issuer: the name of the class or function in which the
         error occurred.
  */
};

class MemFreeExc: public Exception {
  /*:C MemFreeExc
   *:D A descendent of Exception that reports memory freeing failures.
   *:N Note that an error during freeing of memory indicates a severe bug.
   */
public:
  MemFreeExc(QString issuer="");
  /*:F MemFreeExc(QString issuer="")
   *:A QString issuer: the name of the class or function in which the
         error occurred.
  */
};

template <class X> inline X *memalloc(int n, QString requestor) {
  try {
    return new X[n];
  } catch (...) {
    throw MemExc(requestor);
  }
}


#endif
