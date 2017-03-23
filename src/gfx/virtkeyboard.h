// gfx/virtkeyboard.h - This file is part of VScope.
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
