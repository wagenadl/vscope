// filechooser.h

#ifndef FILECHOOSER_H

#define FILECHOOSER_H

#include <QFrame>
#include <QDir>

class FileChooser: public QFrame {
  /*:C FileChooser
   *:D This is a frame around a FileList, with Left and Right buttons
       and perhaps an Up button.
       The geometry of the FileChooser is typically set by the parent;
       the FileChooser then picks an appropriate size for the FileList.
  */
  Q_OBJECT;
public:
  FileChooser(QWidget *parent, class FileList *alt_child=0);
  /*:F constructor
   *:D Optional second argument passes in a preconstructed file list.
       This is useful if you need a derived class as your file list, e.g.,
       to employ more complex file name inclusion rules or to show additional
       information below the file name.
   *:N This FileChooser becomes the owner of the alt_child, and will 
       ultimately delete it.
  */
  virtual ~FileChooser();
  void populateFiles(QDir const &src, QString extn, bool hideextn=true);
  void populateDirs(QDir const &src);
  int countColumns() const;
  int countItems() const;
  void setBackground(QColor bg);
  class Button *buttonp(QString fn);
  /*:F buttonp
   *:D Returns the button associated with the given filename, or null.
   */
signals:
  void clickedItem(QString fn);
  /*:S clickedItem
   *:D Emitted whenever the user clicks on an item.
   *:N Special case: the "Up" button generates a clickedItem with fn="..".
   */
public slots:
  void clickedLeft();
  void clickedRight();
  void clickedUp();
  void clickedFile(QString fn);
  void scroll(double col);
  void scrolling(double col);
  void scrollTo(QString partialfn="");
protected:
  void resizeEvent(class QResizeEvent *);
private:
  void relayout();
private:
  class FileList *list;
  class Button *buttonLeft, *buttonRight, *buttonUp;
  class HScrollBar *scrollBar;
  bool isDirs;
};

#endif
