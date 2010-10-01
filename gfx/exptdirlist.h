// exptdirlist.h

#ifndef EXPTDIRLIST_H

#define EXPTDIRLIST_H

#include <gfx/filelist.h>
class ExptDirList: public FileList {
  /*:C ExptDirList
   *:D A refinement of FileList that provides additional info for those
       directories that contain experiments.
   *:N We use the "log.txt" file to determine whether a directory contains
       an experiment; the presence of "Trial nnn: xxxx" lines in that file
       is our heuristic.
  */
  Q_OBJECT;
public:
  ExptDirList(QWidget *parent);
protected:
  virtual QString infoHook(class QDir const &dir, QString fn);
};

#endif
