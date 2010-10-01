// pvpBase.cpp

#include "pvpBase.h"

#include <pvp/dwpvcam.h>
#include <stdio.h>
#include <base/dbg.h>

// ======================= pvpException =================================
pvpException::pvpException(QString issuer, QString aux):
  Exception(issuer,"",aux) {
  char pvcam_buffer[ERROR_MSG_LEN];
  if (pl_error_code()==0) {
    addMessage("No pvcam hardware");
  } else if (pl_error_message(pl_error_code(),pvcam_buffer)) {
    addMessage(pvcam_buffer);
  } else {
    QString msg = "Cannot obtain pvcam error message (";
    try {
      msg += QString::number(pl_error_code());
    } catch(...) {
    }
    msg += ")";
    addMessage(msg);
  }
  if (immrep)
    report();
}

char const *pvpAccess::decode() {
  switch (x) {
  case Error: return "Error";
  case ReadOnly: return "ReadOnly";
  case ReadWrite: return "ReadWrite";
  case ExistCheckOnly: return "ExistCheckOnly";
  case WriteOnly: return "WriteOnly";
  default: return "UNKNOWN";
  }
}
