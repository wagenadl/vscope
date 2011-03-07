// analogdata.h

#ifndef ANALOGDATA_H

#define ANALOGDATA_H

#include <base/types.h>
#include <base/exception.h>
#include <QMap>
#include <QHash>
#include <QVector>
#include <base/keyaccess.h>
#include <QDomElement>

class AnalogData: public KeyAccess {
  /*:C AnalogData
   *:D This class holds any amount of analog data from AnalogIn or for
       AnalogOut. It keeps track of the physical channels with which
       the data are associated, but not their gain and other settings.
  */
public:
  AnalogData(int nscans, int nchannels, double fs_hz);
  /*:F AnalogData(int nscans, int nchannels)
   *:D This constructor allocates space for multiple scans of data from
       multiple channels.
   *:A nscans: number of scans (i.e. multichannel samples)
    :  nchannels: number of channels
  */
  ~AnalogData();
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
  void defineChannel(int index, QString channel,
		     double scale=1, QString unit="V");
  /*:F void defineChannel(int index, int channel)
   *:D Specifies mapping for one physical channel.
   *:A int index: index within the scan.
    :  QString channel: physical channel to associate with that index.
    :  scale, unit: conversion from DAQ units to logical units.
  */
  typedef QMap<QString, double> ScaleMap;
  void write(QString ofn, QDomElement elt);
  ScaleMap writeInt16(QString ofn);
  /*:F writeInt16
   *:D Writes data to a new file in a compact binary file format.
   *:R Value of digital steps for each channel (in DAQ input Volts).
   */
  void read(QString ifn, QDomElement elt);
  void readInt16(QString ifn, ScaleMap const &steps);
  /*:F readInt16
   *:D Reads data from a file created by writeInt16(). Unlike read(), this
       requires that the channel map has already been set up.
  */
  double const *channelData(QString channel) const;
  double *channelData(WriteKey *, QString channel);
  /*:F channelData(int channel)
   *:D Returns a pointer to the data associated with a given physical channel.
   *:A int channel: Physical channel number
   *:R Pointer to data.
   *:N Note that the data are interleaved, so that the second scan is located
       getNumChannels() floats after the first scan.
    :  Behavior is undefined if channel is not part of the channel list.
  */
  double const *allData() const;
  double *allData(WriteKey *);
  /*:F allData
   *:D Returns a pointer to all data, that is, to the first channel in the
       first scan.
   *:R Pointer to data.
   *:N There are getNumScans() * getNumChannels() doubles at the location
       returned.
  */
  int getNumScans() const { return nscans; }
  /*:F getNumScans
   *:D Return the actual number of scans in this chunk.
   */
  int getNumChannels() const { return nchannels; }
  /*:F getNumChannels
   *:D Return the number of channels represented in this chunk.
   */
  double getSamplingFrequency() const { return fs_hz; }
  QString getChannelAtIndex(int n) const;
  /*:F int getChannelAtIndex
   *:D Return which physical channel is represented at a particular index
       within each scan.
   *:A int n: index into scan.
   *:R Physical channel name.
   */
  double getScaleAtIndex(int n) const;
  QString getUnitAtIndex(int n) const;
  bool contains(QString ch) const;
  /*:F contains
   *:D Returns true iff the given channel is included in this dataset.
   */
  int whereIsChannel(QString ch) const;
  /*:F whereIsChannel
   *:D Returns the index of the given channel, or -1 if not included.
   */
private:
  int32_t nscans;
  int32_t nchannels;
  double fs_hz;
  int32_t ndoubles_allocated;
  QVector<QString> index2id;
  QVector<double> index2scale;
  QVector<QString> index2unit;
  QHash<QString, int32_t> id2index;
  double *data; // by index
};

#endif
