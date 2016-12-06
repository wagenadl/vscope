// exception.cpp

#include <base/exception.h>
#include <base/dbg.h>
#include <base/object.h>
#include <QQueue>

#include <errno.h>

#ifdef vsdLINUX
#include <execinfo.h>
#include <cxxabi.h>
#endif
#include <stdlib.h>

bool Exception::immrep = true;

void Exception::immediateReport(bool onof) {
  immrep = onof;
}

Exception::Exception(QString issuer_, QString msg_, QString aux_) {
  construct(issuer_, msg_, aux_);
}

Exception::Exception(QObject const *issuer_, QString msg_, QString aux_) {
  construct(objName(issuer_), msg_, aux_);
}

void Exception::construct(QString issuer_, QString msg_, QString aux_) {
  backtrace_data = 0;
  backtrace_count = 0;
  issuer = issuer_;
  if (!aux_.isEmpty()) {
    issuer += " (";
    issuer += aux_;
    issuer += ")";
  }
  message = msg_;

#ifdef vsdLINUX
  try {
    backtrace_data = new void*[1024];
    backtrace_count = backtrace(backtrace_data,1024);
  } catch (...) {
    backtrace_data = 0;
  }
#endif

  if (immrep && message!="")
    report();
}

Exception::Exception(Exception const &e) {
  issuer = e.issuer;
  message = e.message;
  backtrace_data = 0;
  if (e.backtrace_data) {
    try {
      backtrace_data = new void *[backtrace_count=e.backtrace_count];
      memcpy(backtrace_data,e.backtrace_data,backtrace_count*sizeof(void*));
    } catch (...) {
      backtrace_data = 0;
    }
  }
}

Exception::~Exception() {
  if (backtrace_data) {
    delete [] backtrace_data;
    backtrace_data = 0;
  }
}

void Exception::addMessage(QString msg) {
  if (message.isEmpty())
    message = msg;
  else {
    message += ": ";
    message += msg;
  }
}

void Exception::report() const {
  dbg("Exception: %s: %s",qPrintable(issuer),qPrintable(message));
  if (!backtrace_data || backtrace_count<=0) {
    Dbg() << "  No backtrace information available.";
  } else {
    Dbg() << "  Backtrace information:";
#ifdef vsdLINUX
    char **symbols = backtrace_symbols(backtrace_data, backtrace_count);
#else
    char **symbols = 0;
#endif
    size_t dm_length = 1024;
    char *demangled;
    try {
      demangled = new char[dm_length];
    } catch (...) {
      demangled = 0;
    }
    if (!demangled) {
      Dbg() << "    Cannot allocate memory to demangle. Raw data follows.";
      for (int k=0; k<backtrace_count; k++) 
	Dbg() << "    " << symbols[k];
    } else {
      for (int k=0; k<backtrace_count; k++) {
    int status = -1;
	char *begin_name=0, *begin_offset=0, *end_offset=0;
	for (char *p=symbols[k]; *p; ++p) {
	  if (*p=='(')
	    begin_name=p;
	  else if (*p=='+')
	    begin_offset=p;
	  else if (*p==')' && begin_offset && !end_offset)
	    end_offset=p;
	}
	if (begin_name && begin_offset && end_offset
	    && begin_name<begin_offset) {
	  *begin_name++ = *begin_offset++ = *end_offset = 0;
#ifdef vsdLINUX
	  demangled = abi::__cxa_demangle(begin_name, demangled,
					  &dm_length,&status);
#else
	  demangled = 0;
#endif
	  if (status==0) {
	    Dbg() << "    " << demangled << " +" << begin_offset;
	  } else {
	    *--begin_name='(';
	    *--begin_offset='+';
	    *end_offset=')';
	    Dbg() << "    " << symbols[k];// << " (n.d.)";
	  }
	} else {
	  Dbg() << "    " << symbols[k];// << " (n.d.)";
	}
      }
    }
    if (symbols)
      free(symbols);
    if (demangled)
      delete [] demangled ;
  }
  Dbg() << "  End of backtrace.";
}

Exception::operator QString() const {
  return QString("%1: %2").arg(issuer).arg(message);
}

SysExc::SysExc(QString issuer, QString msg, QString aux):
  Exception(issuer, msg, aux) {
  addMessage(strerror(errno));
}

