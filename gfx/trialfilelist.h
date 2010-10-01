// trialfilelist.h

#ifndef TRIALFILELIST_H

#define TRIALFILELIST_H

#include <gfx/filelist.h>

class TrialFileList: public FileList {
  /*:C TrialFileList
   *:D Refinement of FileList that avoids "-rois.xml" files and that
       supplies the type of trial as aux. info.
  */
  Q_OBJECT;
public:
  TrialFileList(QWidget *parent);
protected:
  virtual bool includeFileHook(class QDir const &dir, QString fn);
  virtual QString infoHook(class QDir const &dir, QString fn);
};

#endif
