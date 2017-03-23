// pvp/pvpException.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// pvpException.cpp

#include "pvpException.h"

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
