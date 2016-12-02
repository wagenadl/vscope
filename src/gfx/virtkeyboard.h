// virtkeyboard.h

#ifndef VIRTKEYBOARD_H

#define VIRTKEYBOARD_H

#include <QFrame>
#include <QMap>
#include <QChar>
#include <QVector>

class VirtKeyboard: public QFrame {
  /*:C VirtKeyboard
   *:D Virtual keyboard for Qt
   */
  Q_OBJECT;
public:
  VirtKeyboard(QWidget *parent);
  virtual ~VirtKeyboard();
  void resizeEvent(class QResizeEvent *);
private:
  void relayout();
  void unshift();
signals:
  void keyPress(QChar asc);
  /* In addition to printable codes, we generate:
       Backspace: 8
       Tab: 9
       Enter: 10
       Left: 21
       Right: 22
       Home: 25
       End: 26
       Esc: 27
       Delete: 127
  */
private:
  class KeyCap {
  public:
    double left;
    double width;
    QChar lower;
    QChar upper;
    class QPushButton *widget;
  };
  typedef QVector<KeyCap> KeyRow;
  QVector<KeyRow *> rows;
  bool shifted;
  bool capslock;
  QMap<QObject *,QChar> lowers;
  QMap<QObject *,QChar> uppers;
  QVector<class QPushButton *> shiftkeys;
  QPushButton *capskey;
private slots:
  void clickedKey();
  void clickedCapsLock();
  void clickedTab();
  void clickedShift();
  void clickedEnter();
  void clickedSpace();
  void clickedBackspace();
  void clickedDelete();
  void clickedLeft();
  void clickedRight();
  void clickedHome();
  void clickedEnd();
  void clickedEscape();
};

#endif
