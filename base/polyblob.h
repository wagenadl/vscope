// polyblob.h

#ifndef POLYBLOB_H

#define POLYBLOB_H

#include <base/exception.h>
#include <stdio.h>
#include <QDomElement>
#include <QVector>
#include <QLineF>

class PolyBlob {
  /*:C PolyBlob
   *:D A PolyBlob is a polygon with lots of vertices that can be molded
       like an amoeba. The vertices are nominally at equal angular intervals
       around a center, but this is not strictly enforced. Distortion happens
       by specifying that a line segment ((x1,y1)-(x2,y2)) is part of the
       edge; this then causes the relevant vertices of the polygon to be
       updated.
   *:N The current implementation stores points as short integers scaled
       50x. This means that all coordinates must be restricted to -655..+655.
   */
public:
  PolyBlob(int log2n=6);
  /*:F constructor
   *:D The argument sets the number of vertices to 2^log2n.
   */
  PolyBlob(PolyBlob const &other);
  virtual ~PolyBlob();
  PolyBlob &operator=(PolyBlob const &other);
  PolyBlob(QDomElement doc);
  /*:F constructor
   *:D Data is read from the given xml
   */
  void revertex(int log2n);
  /*:F revertex
   *:D Change the number of vertices. The current polygon is interpolated
       to the new vertex count.
   */
  int write(FILE *ofd) const;
  /*:F write
   *:D Writes the polygon in raw form to the given file.
   *:R Number of bytes written.
   *:N This does *not* write the vertex count.
   */
  void write(QDomElement doc) const;
  /*:F write
   *:D Writes the polygon to an xml document. The element normally must
       be a <roi> without any content or attributes. Any previous contents
       are cleared.
   *:N The result will be an element that looks like this:
         <roi n="64">
	 x1 y1
	 x2 y2
	 ...
	 x64 y64
	 </roi>
       Numbers are written to two digits after the decimal point.
   */
  int read(FILE *ifd);
  /*:F read
   *:D Reads polygon data in raw form from the given file.
   *:R Number of bytes read.
   *:N Since the vertex count is not stored in the file, some care should
       be taken to ensure consistency.
   */
  void read(QDomElement doc);
  /*:F Read
   *:D Reads polygon data from an xml element
   */
  void build(int k, double *xx, double *yy);
  /*:F build
   *:D Build a polygon from a set of k vertices. Interpolation to 2^log2n
       vertices is performed internally.
   */
  void set(int i, double x, double y);
  /*:F set
   *:D Directly changes one specific vertex.
   */
  void set(int i, QPointF xy);
  double x(int i) const;
  /*:F x
   *:D Return the x-coordinate of one vertex.
   */
  double y(int i) const;
  /*:F y
   *:D Return the y-coordinate of one vertex.
   */
  double x0() const;
  /*:F x0
   *:D Return the x-coordinate of the center of mass of the blob.
   */
  double y0() const;
  /*:F y0
   *:D Return the y-coordinate of the center of mass of the blob.
   */
  void adjust(double x, double y, bool first);
  void adjust(QPointF xy, bool first);
  void reshape(double x1, double y1, double x2, double y2);
  void reshape(QPointF xy1, QPointF xy2);
  void reshape(QLineF xy12);
  /*:F reshape
   *:D Reshapes the polygon by defining the line segment ((x1,y1)-(x2,y2))
       to be on its boundary.
  */
  void recenter(double x0new, double y0new);
  void recenter(QPointF xy0new);
  /*:F recenter
   *:D Moves the polygon (as a whole) to a new center.
   */
  double distToCenter(double x, double y) const;
  double distToCenter(QPointF xy) const;
  /*:F distToCenter
   *:D Returns the distance of the given point to the center of the blob.
   *:N The center of the blob is defined as the average of the positions
       of all vertices. This is not necessarily the same as the center of
       mass of all pixels inside the blob.
   *:N This function is fast; run time is O(1), i.e., not dependent on log2n.
   */
  double distToEdge(double x, double y) const;
  double distToEdge(QPointF xy) const;
  /*:F distToEdge
   *:D Returns the distance of the given point to the nearest vertex.
   *:N "Nearest vertex" means the vertex with the closest-matching angle
       from the center; this may therefore be slightly more than the
       exact orthogonal distance to the edge.
   *:N This function is fast; run time is O(1), i.e., not dependent on log2n.
   */
  double greatestRadius() const;
  /*:F greatestRadius
   *:D Returns the dist. to center of the vertex farthest away from the
       center.
   *:N This is not fast; we need to consider every single vertex.
   */
  bool inside(double x, double y, double margin=0) const;
  /*:F inside
   *:D Returns true iff the given point lies inside the blob, or at most
       MARGIN pixels outside of it.
   *:N This uses a heuristic based on the "nearest vertex" concept of
       distToEdge().
   */
  bool inside(QPointF xy, double margin=0) const;
  double weight(double x, double y, double borderwidth) const;
  double weight(QPointF xy, double borderwidth) const;
  /*:F weight
   *:D Returns the weight (0..1) of a pixel inside or outside the blob.
       Pixels well inside have weight 1, those well outside have weight 0,
       and ones near the border have intermediate weights.
  */
  double binaryScale() const;
  /*:F binaryScale
   *:D Returns the scale factor applied before saving as binary.
   */
  int binaryTypeSize() const;
  /*:F binaryTypeSize
   *:D Returns the number of bytes of each value in the binary files.
   */
  int nPoints() const;
  /*:F nPoints
   *:D Returns the number of points (i.e. vertices) in this blob.
   */
  int log2nPoints() const;
  /*:F log2nPoints
   *:D Returns the log base 2 of the number of points (i.e. vertices)
       in this blob.
   */
  void paint(class QPainter *pntr) const;
  void paint(class QPainter *pntr, class Transform const &t) const;
  /*:F paint
   *:D Renders this polyblob with the current pen onto a drawable using
       the given painter.
  */
private:
  int log2n;
  int n;
  int msk;
  QVector<short> x50;
  QVector<short> y50;
  int sumx;
  int sumy;
private:
  void recalc_center(); // recalculates sumx, sumy.
private:
  double lastx, lasty, lastr, lastphi;
  int lastk;
};

#endif
