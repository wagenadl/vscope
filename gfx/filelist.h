// filelist.h

#ifndef FILELIST_H

#define FILELIST_H

#include <QFrame>
#include <QString>
#include <QStringList>
#include <QColor>

/* A FileDialog consists of one or two FileChoosers: one for directories,
   and one for files. It also may contain a bar for direct typing of a
   name, OK and Cancel buttons, and a VirtKeyboard.
   A FileChooser consists of a FileList, "Left" and "Right" buttons to navigate
   that list, and an "Up" button if the chooser contains directories.
*/

class FileList: public QFrame {
  /*:C FileList
   *:D A simple array of file or directory names arranged in columns.
       This class does not provide scroll bars; that is left to the next
       level up, which is called FileChooser.
  */
  Q_OBJECT;
public:
  FileList(QWidget *parent);
  virtual ~FileList();
  void setColumnWidth(int pix);
  void setRowHeight(int pix);
  void setHeight(int pix);
  int countColumns() const { return ncols; }
  int countItems() const { return children.size(); }
  int columnWidth() const { return colwidth_pix; }
  int rowHeight() const { return rowheight_pix; }
  int countRows() { return nrows; }
  void populateFiles(class QDir const &src, QString extn, bool hideext=true);
  void populateDirs(class QDir const &src);
  void populate(class QDir const &src, QStringList lst);
  int indexOf(QString partialfn); // returns the index of the first entry matching partialfn, or -1
  void setBackground(QColor bg);
protected:
  virtual bool includeFileHook(class QDir const &dir, QString fn);
  /*:F includeFileHook
   *:D Hook for descendents to vet whether a certain file should be included
       in the list.
   *:A fn is relative to our directory.
   *:N The default implementation returns true unless fn starts with "_".
   */
  virtual bool includeDirHook(class QDir const &dir, QString fn);
  /*:F includeDirHook
   *:D Hook for descendents to vet whether a certain dir should be included
       in the list.
   *:A fn is relative to our directory.
   *:N The default implementation returns true unless fn starts with "_".
  */
  virtual QString infoHook(class QDir const &dir, QString fn);
  /*:F infoHook
   *:D Hook for descendents to supply additional information about a file.
   *:A fn is relative to our directory.
   *:N The default implementation returns the empty string.
   */
signals:
  void clickedItem(QString fn);
private:
  void relayout();
private slots:
  void buttonClicked(QString id, QString txt);
private:
  int colwidth_pix;
  int rowheight_pix;
  int height_pix;
  int width_pix; // automatically calculated
  int nrows;
  int ncols;
  QVector<class Button *> children;
  QColor bg;
};

#endif
