// exptfiledialog.cpp

#include "exptfiledialog.h"
#include "trialfilelist.h"
#include "exptdirlist.h"
#include "filechooser.h"
#include <base/dbg.h>

ExptFileDialog::ExptFileDialog(QWidget *parent):
  FileDialog(parent,
	     new FileChooser(parent, new ExptDirList(parent)),
	     new FileChooser(parent, new TrialFileList(parent))) {
  dbg("exptfiledialog");
}
