// traceinfo.h - Information about a single trace of a graph

#ifndef Traceinfo_H
#define Traceinfo_H

#include <base/range.h>
#include <base/types.h>
#include <QString>
// QString included just to force definition of intXX_t types

class TraceInfo {
  /*:C TraceInfo
   *:D Container for data used in a LineGraph.
       The data can be of many integer or floating point types; this
       is largely transparent to callers.
  */
 public:
  enum DataType { dataNone=0, 
		  dataInt8, dataUInt8, 
		  dataInt16, dataUInt16,
		  dataInt32, dataUInt32,
		  dataFloat, dataDouble,
		  dataBinary };
  union DataPtr {
    void const *dp_none;
    int8_t const *dp_int8;
    uint8_t const *dp_uint8;
    int16_t const *dp_int16;
    uint16_t const *dp_uint16;
    int32_t const *dp_int32;
    uint32_t const *dp_uint32;
    float const *dp_float;
    double const *dp_double;
    uint32_t const *dp_binary;
  };
 public:
  TraceInfo(DataType dt=dataNone);
  TraceInfo(DataType dt, int bit);
  double getX0() const { return datax0; }
  double getDX() const { return datadx; }
  double getDatum(int n) const;
  int getN() const { return N; }
  void setData(double datax0, double datadx, DataPtr dp, int N, int step=1);
  /*:F setData
   *:D Associates new data with this trace. The data at DP are not copied,
       and must remain available until setData is called again or this
       TraceInfo is destroyed.
   *:A datax0: x-coordinate associated with first data point
       datadx: x-interval between adjacent data points
       dp: pointer to actual data
       N: number of points
       step: number of channels in data set (used only to skip the other channels)
   *:N This does not cause an update of any graphs.
  */
  void setScaleFactor(double scale=1);
  /*:F setScaleFactor
   *:D When set to a number other than one, data are scaled when read by
       range(), range99() or trueBlue().
   *:N E.g., if the data at DATAPTR consists of the numbers 2 and 3,
       and SCALE=10, then range() would return [20,30].
  */
  void setOffset(double offset=0);
  /*:F setOffset
   *:D This offset is added to return values of range(), range99() and
       trueBlue(). Offsets are applied after scaling.
  */
  Range range() const;
  /*:F range
   *:D Full range of the data, scaled.
   */
  Range range99(double frc0=.01, double frc1=0) const;
  /*:F range99
   *:D Fractile range of the data, scaled.
       This returns the frc0-th and frc1-th fractiles of the data.
       If frc1 is not given, it defaults to 1-frc0.
  */
  Range zoomrange99(double x0, double x1, double frc0=.01, double frc1=0) const;
   /*:F zoomrange99
    *:D As range99, except that arguments x0 and x1 specify that we're
        looking at a subset of the data.
  */
  void trueBlue(double x0, double dx, int M,
		double *outmin, double *outmax) const;
  /*:F trueBlue
   *:D Minima and maxima in bins, scaled.
       For M bins, [x0,x0+dx), [x0+dx,x0+2*dx), ... [x0+(M-1)*dx,x1+M*dx),
       this calculates the min and max of the data within each bin.
       If there are 0 elements in the bin, the next element is used, unless
       there are no data at all.
       The output is scaled by the current scale factor.
  */
private:
  Range range99(int n0, int n1, double frc0, double frc1) const;
 protected: 
  friend class LineGraph; // read only!
  DataType datatype;
  int bit; // for binary data only
  double datax0;
  double datadx;
  DataPtr dataptr;
  int N;
  int step;
  double scale;
  double offset;
};

#endif
