// toplevel/mgauto.h - This file is part of VScope.
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

// mgauto.h

#ifndef MGAUTO_H

#define MGAUTO_H

#include <gfx/multigraph.h>
#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include <QDomElement>

class MGAuto: public MultiGraph {
  /*:C MGAuto
   *:D Multigraph set that reads from XML which channels to display
   */
  Q_OBJECT;
public:
  MGAuto(QWidget *parent, QString myname);
  /*:F constructor
   *:D Creates the set. 
  */
  virtual ~MGAuto();
public slots:
  void rebuild();
private:
  void newgraphs();
  void newtraces();
  bool useInputChannel(QString id) const;
  bool useOutputChannel(QString id) const;
private:
  QString myname;
  QList<QString> actual;
  QSet<QString> digiSet;
  QSet<QString> outputSet;
  QMap<QString, class TraceInfo *> traces;
};

#endif
