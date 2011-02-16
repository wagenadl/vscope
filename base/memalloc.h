// memalloc.h

#ifndef MEMALLOC_H

#define MEMALLOC_H

#include <base/exception.h>

#define MEMALLOC_MAXBYTES (1024*1024*1024)

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
    if (n<=0)
      throw MemExc(requestor +
		   ": Requesting non-positive amount of memory" +
		   QString(" (%1 x %2 bytes).").arg(n).arg(sizeof(X)));
    if (n*sizeof(X) > MEMALLOC_MAXBYTES)
      throw MemExc(requestor +
		   ": Requesting unreasonable amount of memory" +
		   QString(" (%1 x %2 bytes).").arg(n).arg(sizeof(X)) +
		   " Change MEMALLOC_MAXBYTES in memalloc.h if you disagree.");
    return new X[n];
  } catch (...) {
    throw MemExc(requestor);
  }
}


#endif
