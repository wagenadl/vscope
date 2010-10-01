// exception.h

#ifndef GUIEXCEPTION_H

#define GUIEXCEPTION_H

#include <QString>
#include <base/exception.h>

extern void report(Exception const &e, QString reporter);
extern void guiwarn(QString txt);
#endif
