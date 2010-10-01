// dbg.cpp

#include <base/dbg.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <QDateTime>

//class DBG_T {
// public:
//  DBG_T() {
//    gettimeofday(&tv,0);
//  }
//  struct timeval tv;
//};
//
//DBG_T dbg_t0;

void warn(char const *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  fprintf(stderr,"WARNING: ");
  vfprintf(stderr,fmt,ap);
  int n=strlen(fmt);
  if (n==0 || fmt[n-1]!='\n')
    fprintf(stderr,"\n");
  va_end(ap);
}


void dbg(char const *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  fprintf(stderr,"[%s] ",
	  qPrintable(QTime::currentTime().toString("hhmmss.zzz")));
  vfprintf(stderr,fmt,ap);
  int n=strlen(fmt);
  if (n==0 || fmt[n-1]!='\n')
    fprintf(stderr,"\n");
  va_end(ap);
}

#include <QTime>

Dbg::Dbg() {
  txt = "[" + QTime::currentTime().toString("hhmmss.zzz") + "] ";
  setString(&txt);
}

Dbg::~Dbg() {
  if (!txt.endsWith("\n"))
    txt += "\n";
    
  fprintf(stderr,"%s",qPrintable(txt));
}
