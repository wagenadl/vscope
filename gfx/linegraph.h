// linegraph.h - widget to hold one or more curves and a system of axes

#ifndef Linegraph_H
#define Linegraph_H

#include <QFrame>
#include <QString>
#include <QMap>
#include <QFont>
#include <QPen>
#include <QColor>
#include <QPoint>

#include <gfx/traceinfo.h>
#include <base/range.h>

class LineGraph: public QFrame {
  /*:C LineGraph
   *:D This widget shows a single pair of x- and y-axes with any number of
       line graphs shown on them.
  */
  Q_OBJECT;
 public:
  LineGraph(QWidget *parent);
  virtual ~LineGraph();
  void setAutoRepaint(bool ar);
  /*:F setAutoRepaint
   *:D Determines whether or not the graph gets repainted or not for
       every call to functions that may change the appearance.
   *:N The default is yes. If you unset autorepaint, use update() to force
       a redraw.
  */
  bool getAutoRepaint() const { return autoRepaint; }
  /*:F getAutoRepaint
   *:D Reports whether or not the graph gets repainted or not for
       every call to functions that may change the appearance.
  */
  void setXRange(Range const &xx, bool nextAuto=false);
  /*:F setXRange
   *:D Set the limits for the x-axis.
   *:A If nextAuto is set to true, we will pretend that the given range
       was the result of autoSetXRange.
   */
  void setYRange(Range const &yy);
  /*:F setYRange
   *:D Set the limits for the y-axis.
   */
  Range getXRange() const;
  /*:F getXRange
   *:D Returns the current limits for the x-axis.
   *:N This is mainly useful if the range was set using any of the autoXXX
       functions.
  */
  Range getYRange() const;
  /*:F getYRange
   *:D Returns the current limits for the y-axis.
   *:N This is mainly useful if the range was set using any of the autoXXX
       functions.
  */
  void autoSetXRange();
  /*:F autoSetXRange
   *:D Automatically determines x-axis limits based on the data currently
       in the graph. This sets the edges to exactly match the limits of
       the data.
   *:N As a special case, if there is no data, this sets the x-axis
       to display the interval [0,1].
  */
  void autoSetYRange(double frc=0, double margfac=1e-3);
  /*:F autoSetYRange
   *:D Automatically determines y-axis limits based on the data currently
       in the graph.
   *:A frc: what fraction of data is allowed to be out-of-range.
       When called without arguments, this sets the edges to exactly match
       the limits of the data.
   *:N As a special case, if there is no data, this sets the y-axis
       to display the interval [-1,1].
  */
  void autoGrowXRange();
  /*:F autoGrowXRange
   *:D Expands the x-axis limits based on current data but does not shrink it.
   */
  void autoGrowYRange(double frc=0);
  /*:F autoGrowYRange
   *:D Expands the y-axis limits based on current data but does not shrink it.
   *:A frc: what fraction of data is allowed to be out-of-range.
       When called without arguments, this sets the edges to exactly match
       the limits of the data.
   */
  bool isXRangeAuto() const { return xrangeFromAuto; }
  /*:F isXRangeAuto
   *:D Returns true if the current x-axis range was determines by any of
       the autoXXX functions.
  */
  bool isYRangeAuto() const { return yrangeFromAuto; }
  /*:F isYRangeAuto
   *:D Returns true if the current y-axis range was determines by any of
       the autoXXX functions.
  */
  void setXTicks(double dx=0, double dxm=0);
  /*:F setXTicks
   *:D Set the spacing between ticks on the x-axis. Normally, major ticks
       get labeled, minor ticks do not.
   *:A Setting dx or dxm to 0 reverts to automatic determination based on
       axis limits. To remove minor ticks, set the minor tick length to zero.
  */
  void setYTicks(double dy=0, double dym=0); 
  /*:F setYTicks
   *:D Set the spacing between ticks on the y-axis. Normally, major ticks
       get labeled, minor ticks do not.
   *:A Setting dy or dym to 0 reverts to automatic determination based on
       axis limits. To remove minor ticks, set the minor tick length to zero.
  */
  double getXTicks() const;
  /*:F getXTicks
   *:D Return current spacing between major x-axis ticks.
   */
  double getYTicks() const;
  /*:F getYTicks
   *:D Return current spacing between major y-axis ticks.
   */
  double getMinorXTicks() const;
  /*:F getMinorXTicks
   *:D Return current spacing between minor x-axis ticks.
   */
  double getMinorYTicks() const;
  /*:F getMinorYTicks
   *:D Return current spacing between minor y-axis ticks.
   */
  void setAxisFont(QFont const &f);
  /*:F setAxisFont
   *:D Set the font used for writing axis labels.
   */
  class QFont const &getAxisFont() const { return axisFont; }
  /*:F getAxisFont
   *:D Retrieve the font used for writing axis labels.
   */
  void setTraceFont(QFont const &f);
  /*:F setTraceFont
   *:D Sets the font used for writing trace keys.
   */
  class QFont const &getTraceFont() const { return traceFont; }
  /*:F getTraceFont
   *:D Retrieves the font used for writing trace keys.
   */
  void setBackgroundColor(QColor const &c);
  /*:F setBackgroundColor
   *:D Sets the background color of the graph. The default is white.
   */
  QColor const &getBackgroundColor() const { return backgroundColor; }
  /*:F getBackgroundColor
   *:D Retrieves the background color of the graph.
   */
  void setAxisPen(QPen const &p);
  /*:F setAxisPen
   *:D Sets the pen used to draw axes and ticks. The default is a thin gray.
   */
  class QPen const &getAxisPen() const { return axisPen; }
  /*:F getAxisPen
   *:D Retrieves the pen used to draw axes and ticks.
   */
  void setXLabel(QString lbl);
  /*:F setXLabel
   *:D Sets a label to be printed at the rightmost end of the x-axis.
   */
  void setYLabel(QString lbl);
  /*:F setYLabel
   *:D Sets a label to be printed at the top of the y-axis.
   */
  void setAxisTextColor(QColor const &c);
  /*:F setAxisTextColor
   *:D Sets the color of axis text (used for tick labels).
       The default is the same grey used for the axis pen.
   *:N To avoid printing axis text, do not set this to the background
       color. Instead, call showXTickLabels(false) or showYTickLabels(false).
   */
  class QColor const &getAxisTextColor() const { return axisTextColor; }
  /*:F getAxisTextColor
   *:D Retrieves the color of axis text.
   */
  void setAxisTickLength(int maj=0, int min=0);
  /*:F setAxisTickLength
   *:D Sets the length (in pixels) of major and minor tick marks.
       0 draws no ticks.
  */
  int getAxisTickLength() const { return axisTickLength; }
  /*:F getAxisTickLength
   *:D Retrieves the current length of major ticks.
   */
  int getAxisMinorTickLength() const { return axisMinorTickLength; }
  /*:F getAxisTickLength
   *:D Retrieves the current length of minor ticks.
   */
  bool addTrace(QString id, TraceInfo const *ti);
  /*:F addTrace
   *:D Adds or replaces a trace to the graph.
   *:A id: identifier for this trace. If id is already in use, the
       corresponding trace gets replaced.
   *:R True if this replaces an existing trace.
   *:N The traceinfo must remain in existence until it is removed from the
       graph! Caller remains owner; LineGraph will never delete a TraceInfo.
   */
  bool removeTrace(QString id);
  /*:F removeTrace
   *:D Removes a trace from the graph.
   *:R Returns true iff trace found.
   *:N Silently fails with zero return value if the trace did not exist.
   */
  void removeAllTraces();
  /*:F removeAllTrace
   *:D Removes all traces from the graph.
   */
  bool setTracePen(QString id, QPen const &p);
  /*:F setTracePen
   *:D Sets the pen used to render a given trace.
       If the trace doesn't exist, this information is stored for future
       use.
   *:R Returns true iff trace found
   */
  QPen getTracePen(QString id) const;
  /*:F getTracePen
   *:D Retrieves the pen used to draw a given trace. If "id" does not
       identify a real trace, returns the pen that would be used if "id"
       came into existence later.
  */
  bool setTraceLabel(QString id, QString label);
  /*:F setTraceLabel
   *:D Sets the label to be written for a given trace.
       If the trace doesn't exist, this information is stored for future
       use.
   *:R Returns true iff trace found
   */
  QString getTraceLabel(QString id) const;
  /*:F getTraceLabel
   *:D Retrieves the label for a given trace. Returns "" if there is no
       such trace.
  */
  void showXTickLabels(bool);
  /*:F showXTickLabels
   *:D Determines whether or not text labels are printed for x-axis ticks.
   */
  void showYTickLabels(bool);
  /*:F showYTickLabels
   *:D Determines whether or not text labels are printed for y-axis ticks.
   */
  void drawLines();
  void drawPoly();
 protected:
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the entire widget.
   */
  double computeFirstXTick();
  /*:F computeFirstXTick
   *:D Computes the data coordinate of the first major tick to be placed
       on the x-axis.
  */
  double computeFirstYTick();
  /*:F computeFirstYTick
   *:D Computes the data coordinate of the first major tick to be placed
       on the y-axis.
  */
  double computeFirstMinorXTick();
  /*:F computeFirstMinorXTick
   *:D Computes the data coordinate of the first minor tick to be placed
       on the x-axis.
  */
  double computeFirstMinorYTick();
  /*:F computeFirstMinorYTick
   *:D Computes the data coordinate of the first minor tick to be placed
       on the y-axis.
  */
  void perhapsRepaint();
  /*:F perhapsRepaint
   *:D Redraws the widget only if autoRepaint is set.
   */
  Range computeXRange() const;
  /*:F computeXRange
   *:D Computes a suitable range for the x-axis given the current data.
   *:N Returns an empty Range if there are no traces.
   */
  Range computeYRange(double frc=0) const;
  /*:F computeYRange
   *:D Computes a suitable range for the y-axis given the current data.
   *:A frc: fraction of points that are allowed to end up off-graph.
   *:N Returns an empty Range if there are no traces.
   */
  int dataToScreenX(double x) const;
  /*:F dataToScreenX
   *:D Converts data x-coordinates to screen pixels.
   */
  int dataToScreenY(double y) const;
  /*:F dataToScreenY
   *:D Converts data y-coordinates to screen pixels.
   */
  double screenToDataX(int x) const;
  /*:F screenToDataX
   *:D Converts screen pixel x-coordinates to data space.
   */
  double screenToDataY(int y) const;
  /*:F screenToDataY
   *:D Converts screen pixel y-coordinates to data space.
   */
  static double reasonableTick(double dx, int fulpix);
  /*:F reasonableTick
   *:D Calculates reasonable spacing (in data coordinates) for major ticks
       given that a total data range DX must be represented in FULPIX pixels.
  */
  static double reasonableMinorTick(double dx, int fulpix);
  /*:F reasonableMinorTick
   *:D Calculates reasonable spacing (in data coordinates) for minor ticks
       given that a total data range DX must be represented in FULPIX pixels.
  */
  static double computeFirstTick(double x0, double x1, double dx, 
				int pix);
  /*:F computeFirstTick
   *:D Calculates the data coordinate of the first tick for an axis of PIX
       pixels, and given tick spacing DX and axis range X0..X1.
       The result is always a multiple of DX.
  */
  static int reasonablePrecision(double dx);
  /*:F reasonablePrecision
   *:D Calculates number of digits needed to show numbers that differ by DX.
   */
  int contentsWidth() const;
  /*:F contentsWidth
   *:D Returns the width of the portion of this widget actually available
       for the graph.
   */
  int contentsHeight() const;
  /*:F contentsHeight
   *:D Returns the height of the portion of this widget actually available
       for the graph.
   */
  int contentsX0() const;
  /*:F contentsX0
   *:D Returns the coordinate of the leftmost point in this widget actually
       available for the graph.
  */
  int contentsY1() const;
  /*:F contentsY1
   *:D Returns the coordinate of the topmost point in this widget actually
       available for the graph.
  */
  void paintXAxis(QPainter &p);
  /*:F paintXAxis
   *:D Draws the x-axis, its ticks, and its labels.
   */
  void paintYAxis(QPainter &p);
  /*:F paintYAxis
   *:D Draws the y-axis, its ticks, and its labels.
   */
  void paintTrace(QPainter &p, class TraceInfo const *ti);
  /*:F paintTrace
   *:D Draws the given trace.
   */
signals:
  void zoomRequest(Range xx);
protected:
  virtual void mousePressEvent(class QMouseEvent *);
  virtual void mouseMoveEvent(class QMouseEvent *);
  virtual void mouseReleaseEvent(class QMouseEvent *);
  virtual void mouseDoubleClickEvent(class QMouseEvent *);
protected:
  enum DrawMode { Poly, Lines };
  QFont traceFont;
  QFont axisFont;
  QPen axisPen;
  QPen lastTracePen;
  QColor backgroundColor;
  QColor axisTextColor;
  int axisTickLength; // pixels
  int axisMinorTickLength; // pixels
  double x0, x1, y0, y1; // actual axis ranges
  double dx, dxm, dy, dym; // major and minor tick distances; 0 for auto
  int labelHTickSpace, labelVTickSpace; 
  int hMargin; // margin (pix) on left and right for traces
  int vMargin; // margin (pix) on top and bottom for traces
  bool autoRepaint;
  bool xrangeFromAuto;
  bool yrangeFromAuto;
  bool xtickLabelsShown;
  bool ytickLabelsShown;
  QString xLabel, yLabel;
protected:
  // Really, this should be a vector or some other ordered thing.
  typedef QMap<QString, TraceInfo const *> Traces;
  Traces traces;
  typedef QMap<QString, QPen> TracePens;
  TracePens tracePens;
  typedef QMap<QString, QString> TraceLabels;
  TraceLabels traceLabels;

  class QRubberBand *rubberband;
  QPoint clickPoint;
  DrawMode drawMode;
};

#endif
