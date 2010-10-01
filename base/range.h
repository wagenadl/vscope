// range.h - data type to represent a range [min,max].

#ifndef Range_H
#define Range_H

#include <base/numbers.h>

class Range {
  /*:C Range
   *:D This class represents a closed interval [min,max] of real numbers.
   *:N The actual edges of the interval are stored in the publicly accessible
       variables min and max.
   */
public:
  Range(): min(numbers.inf), max(-numbers.inf) {
    /*:F constructor
     *:D The default constructor creates an empty interval by setting the
         low end of the range to infinity and the high end to negative
	 infinity.
    */
  }
  Range(double min, double max): min(min), max(max) {
    /*:F constructor
     *:D This form explicitly sets the edges of the interval.
     */
  }
  void include(double x) {
    /*:F include
     *:D If x is not currently inside the interval, the interval is 
         expanded to include it.
    */
    if (x<min)
      min=x;
    if (x>max)
      max=x;
  }
  bool empty() const { return max<min; }
  /*:F empty
   *:D Returns true if the interval is empty.
   */
  void mul(double scale) {
  /*:F mul
   *:D Multiplies the min and max values by SCALE.
   *:N If scale<0 this involves exchanging min and max. This case is handled
       correctly.
   */
    if (scale>0) {
      min*=scale;
      max*=scale;
    } else {
      double m0 = min;
      min = max*scale;
      max = m0*scale;
    }
  }
  void add(double offset) { min+=offset; max+=offset; }
  /*:F add
   *:D Adds OFFSET to the min and max values.
   */
public:
  double min;
  /*:V min
   *:D The low end of the interval.
   */
  double max;
  /*:V max
   *:D The high end of the interval.
   */
};

#endif
