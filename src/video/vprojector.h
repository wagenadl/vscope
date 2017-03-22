// video/vprojector.h - This file is part of VScope.
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

// vprojector.h

#ifndef VPROJECTOR_H

#define VPROJECTOR_H

#include <QObject>

class VProjector: public QObject {
  Q_OBJECT;
public:
  VProjector(QObject *parent=0);
  virtual ~VProjector();
  bool isOk() const;
public slots:
  bool activate();
  void prepare(class ParamTree const *);
  void start();
private slots:
  void readyReadStderr();
  void readyReadStdout();
private:
  class QProcess *proc;
};

#endif
