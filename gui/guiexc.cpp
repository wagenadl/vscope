// guiexc.cpp

#include "guiexc.h"
#include <base/dbg.h>
#include <QMessageBox>

void report(class Exception const &e, QString reporter) {
  dbg("Exception (caught by %s): %s",
      qPrintable(reporter),
      qPrintable(QString(e)));
  if (reporter=="")
    QMessageBox::critical(0, "VScope Exception",
			  QString("Fatal error: %1.").arg(QString(e)),
			  QMessageBox::Close);
  else
    QMessageBox::critical(0, "VScope Exception",
			  QString("Exception (reported by %1): %2.\nIt may be best to restart the application.").arg(reporter).arg(QString(e)),
			  QMessageBox::Ignore);
}

void guiwarn(QString txt) {
  QMessageBox::warning(0, "VScope Warning",
		       txt); 
  //QMessageBox::Ignore);
}
