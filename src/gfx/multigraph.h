// gfx/multigraph.h - This file is part of VScope.
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

// multigraph.h

#ifndef MULTIGRAPH_H

#define MULTIGRAPH_H

#include <QVector>
#include <QFrame>
#include <QString>
#include <QColor>
#include <QMap>
#include <QSet>

#include <base/range.h>

class MultiGraph: public QFrame {
  /*:C MultiGraph
   *:D A MultiGraph is a vertical stack of LineGraphs.
   */
  Q_OBJECT;
public:
  static const int MAXTINYHEIGHT = 24;
  /*:V MAXTINYHEIGHT
   *:D A graph designated as "tiny" will take up no more than MAXTINYHEIGHT
       pixels. It will also never be taller than 1/2 of a normal graph.
  */
public:
  MultiGraph(QWidget *parent);
  /*:C constructor
   *:D Creates a new empty widget.
   */
  virtual ~MultiGraph();
  void addGraph(QString id, class LineGraph *lg, bool tiny=false);
  /*:F addGraph
   *:D Adds a new graph to this multigraph.
       The new graph is added at the bottom of the stack, unless "id" matches
       an existing graph, in which case that existing graph is replaced.
   *:N The graph must have been constructed with us as a parent, otherwise
       geometry setting will not work as expected.
  */
  void setGraphHeight(QString id, int h_pix);
  /*:F setGraphHeight
   *:D Specifies a height for the given graph. This overrides the normal
       layout algorithm which divides the available space evenly.
   *:N The height will be remembered even if the graph is deleted, and
       if a new graph with the same ID is added later, the height will be
       used again. Call resetGraphHeight to undo this.
  */
  void resetGraphHeight(QString id);
  /*:F resetGraphHeight
   *:D Reverts the height of the given graph back to the default: an equal
       share of the available space.
  */
  void removeGraph(QString id);
  /*:F removeGraph
   *:D Removes a graph given its ID.
   */
  void deleteGraph(QString id);
  /*:F deleteGraph
   *:D Removes a graph given its ID and destructs it.
   */
  void clear();
  /*:F clear
   *:D Removes all graphs.
   */
  void deleteAll();
  /*:F deleteAll
   *:D Removes all graphs, and additionally destructs them all.
   */
  class LineGraph *findp(QString id);
  /*:F findp
   *:D Returns a pointer to a graph given its ID, or null if not found.
   */
  class LineGraph &find(QString id);
  /*:F find
   *:D Returns a reference to a graph given its ID, or throws an exception
       if not found.
  */
  QVector<QString> const &getIDs() const { return ids; }
  /*:F getIDs
   *:D Returns the list of all graph IDs.
   */
  void setBackgroundColor(QColor const &c);
  /*:F setBackgroundColor
   *:D Sets the background color of the graphs. The default is gray.
   */
  void connectZooms();
  /*:F connectZooms
   *:D Connects horizontal zoom of this multigraph to all other
       multigraphs registered in this manner.
  */
  void drawLines(QString id);
  void drawPoly(QString id);
public slots:
  void removeGraph(QObject *lg);
  /*:F removeGraph
   *:D Removes a graph given a pointer to the graph.
   *:N This is intended for internal use, mainly if the graph is destructed
       before we are.
  */
protected:
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the entire widget. 
   */
  virtual void resizeEvent(class QResizeEvent *);
  /*:F resizeEvent
   *:D Through this function Qt informs us that we have a new size.
   */
  void newSize();
  /*:F newSize
   *:D Our internal way of updating the stack given a new size.
   */
protected slots:
  void zoomRequest(Range xx);
  void sharedZoomRequest(Range xx);
signals:
  void shareZoom(Range xx);
protected:
  QVector<bool> isTiny;
  QVector<class LineGraph *> graphs;
  QVector<QString> ids;
  QColor backgroundColor;
  QMap<QString, int> forcedHeights;
private:
  static QSet<MultiGraph *> allofus;
};

#endif
