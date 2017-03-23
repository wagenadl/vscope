// gui/timebutton.h - This file is part of VScope.
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

// timebutton.h

#ifndef TIMEBUTTON_H

#define TIMEBUTTON_H

#include <QString>
#include <QTime>
#include <QBasicTimer>
#include <QObject>

class TimeButton: public QObject {
  Q_OBJECT;
public:
  enum Style { None, Wall, Up, Down };
public:
  TimeButton(QObject *parent, class guiButton *button);
  virtual ~TimeButton();
public slots:
  void reset();
  void startCountUp(bool onlyifstopped=false);
  void startCountDown(double time_ms);
  void showWallTime();
  void show();
  void hide();
  void setFormat(QString f);
protected:
  virtual void timerEvent(class QTimerEvent *);
private:
  void relabel();
private:
  Style style;
  QTime time;
  QBasicTimer timer;
  QString caption;
  class guiButton *button;
};

#endif
