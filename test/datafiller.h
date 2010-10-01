// datafiller.h

#ifndef DATAFILLER_H

#define DATAFILLER_H

/* This is an attempt at making a general plugin so that ephysout can
   fill digital data with triggers for video and ccd without specific
   knowledge.
   It would then be up to Trial to put this stuff in.
*/
class DataFiller {
public:
  DataFiller(class ParamTree const &pars);
  virtual int nSamples() const;
  /*:F nSamples
   *:A Number of samples we are planning to fill.
   */
  virtual QSet<int> aChannels() const;
  /*:F aChannels
   *:A A list (possibly empty) of analog channels we are planning to affect.
   *:N This is a promise: fillAData may not touch any other channels.
   */
  virtual QSet<int> dLines() const; 
  /*:F dLines
   *:A A list (possibly empty) of digital lines we are planning to affect.
   *:N This is a promise: fillDData may not touch any other lines.
   */
  virtual void fillAData(class AnalogData *ad) const;
  /*:F fillAData
   *:D Writes appropriate stuff to analog data. May not do anything.
   *:N Caller must first call aChannels() and ensure that all those
       channels specified indeed exist in ad. DataFiller descendents
       should throw an exception if this is violated, but must not,
       of course, crash.
   */
  virtual void fillDData(class DigitalData *dd) const;
  /*:F fillDData
   *:D Writes appropriate stuff to digital data. May not do anything.
   */
protected:
  class ParamTree const &pars;
};

#endif
