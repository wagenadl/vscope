// dbg.h

#ifndef DBG_H
#define DBG_H

#include <QTextStream>
#include <QString>

extern void dbg(char const *fmt,...);
extern void warn(char const *fmt,...);

class Dbg: public QTextStream {
public:
  Dbg();
  virtual ~Dbg();
private:
  QString txt;
};

#endif
