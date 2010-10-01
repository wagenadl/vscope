// pvpException.h

#ifndef PVPEXCEPTION_H

#define PVPEXCEPTION_H

#include <base/exception.h>
#include <QString>

class pvpException: public Exception {
public:
  pvpException(QString issuer="", QString aux="");
};


#endif
