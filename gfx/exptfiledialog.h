// exptfiledialog.h

#ifndef EXPTFILEDIALOG_H

#define EXPTFILEDIALOG_H

#include <gfx/filedialog.h>

class ExptFileDialog: public FileDialog {
  /*:C ExptFileDialog
   *:D A refinement of FileDialog that puts additional information in the
       buttons that represent directories containing experiments, and
       in buttons that represent trials. Also, it suppresses "-rois.xml"
       files.
  */
  Q_OBJECT;
public:
  ExptFileDialog(QWidget *parent);
};

#endif
