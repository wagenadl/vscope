// analogdata.h

#ifndef ANALOGDATA_H

#define ANALOGDATA_H

#include <base/types.h>
#include <base/exception.h>
#include <QMap>
#include <QHash>
#include <QVector>

class AnalogData {
  /*:C AnalogData
   *:D This class holds any amount of analog data from AnalogIn or for
       AnalogOut. It keeps track of the physical channels with which
       the data are associated, but not their gain and other settings.
  */
public:
  AnalogData(int nscans, int nchannels, double fs_hz) throw(Exception);
  /*:F AnalogData(int nscans, int nchannels)
   *:D This constructor allocates space for multiple scans of data from
       multiple channels.
   *:A nscans: number of scans (i.e. multichannel samples)
    :  nchannels: number of channels
  */
  ~AnalogData();
  AnalogData(AnalogData const &other);
  AnalogData &operator=(AnalogData const &other);
  void setSamplingFrequency(double fs_hz);
  void setNumScans(int nscans);
  /*:F setNumScans()
   *:D Specifies the actual number of scans in this chunk. This does not
       reallocate memory, and throws an exception if nscans does not fit
       in the current buffer. See reshape.
   */
  bool reshape(int nscans, int nchannels, bool free=0);
  /*:F reshape
   *:D Reshapes the data buffer. This may claim more space if needed; it
       will not release any unneeded space unless FREE is set to true.
   *:A True if reallocation occurred.
   */
  void defineChannel(int index, int channel) throw(Exception);
  /*:F void defineChannel(int index, int channel)
   *:D Specifies mapping for one physical channel.
   *:A int index: index within the scan.
    :  int channel: physical channel to associate with that index.
  */
  QMap<int,double> writeInt16(QString ofn) throw(Exception);
  /*:F writeInt16
   *:D Writes data to a new file in a compact binary file format.
   *:R Value of digital steps for each channel (in DAQ input Volts).
   */
  void readInt16(QString ifn, QMap<int,double> steps) throw(Exception);
  /*:F readInt16
   *:D Reads data from a file created by writeInt16(). Unlike read(), this
       requires that the channel map has already been set up.
  */
  double *channelData(int channel) const { return data
                                                   + channel2index[channel]; }
  /*:F channelData(int channel)
   *:D Returns a pointer to the data associated with a given physical channel.
   *:A int channel: Physical channel number
   *:R Pointer to data.
   *:N Note that the data are interleaved, so that the second scan is located
       getNumChannels() floats after the first scan.
    :  Behavior is undefined if channel is not part of the channel list.
  */
  double *allData() const { return data; }
  /*:F allData
   *:D Returns a pointer to all data, that is, to the first channel in the
       first scan.
   *:R Pointer to data.
   *:N There are getNumScans() * getNumChannels() doubles at the location
       returned.
  */
  int getNumScans() const { return nscans; }
  /*:F int getNumScans()
   *:D Return the actual number of scans in this chunk.
   */
  int getNumChannels() const { return nchannels; }
  /*:F int getNumChannels()
   *:D Return the number of channels represented in this chunk.
   */
  double getSamplingFrequency() const { return fs_hz; }
  int getChannelAtIndex(int n) const { return (n>=0 && n<nchannels)
                                         ? index2channel[n] : -1; }
  /*:F int getChannelAtIndex(int n)
   *:D Return which physical channel is represented at a particular index
       within each scan.
   *:A int n: index into scan.
   *:R Physical channel number.
   */
  bool contains(int ch) const;
  /*:F contains
   *:D Returns true iff the given channel is included in this dataset.
   */
  int whereIsChannel(int ch) const;
  /*:F whereIsChannel
   *:D Returns the index of the given channel, or -1 if not included.
   */
private:
  int32_t nscans;
  int32_t nchannels;
  double fs_hz;
  int32_t ndoubles_allocated;
  QVector<int32_t> index2channel;
  QHash<int32_t, int32_t> channel2index;
  double *data; // by index
};

#endif
