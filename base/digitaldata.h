// digitalio.h

#ifndef DIGITALDATA_H

#define DIGITALDATA_H

#include <base/exception.h>
#include <base/types.h>
#include <base/keyaccess.h>
#include <QDomElement>
#include <QMap>

class DigitalData: public KeyAccess {
  /*:C DigitalData
   *:D This class holds any amount of digital data from DigitalIn or for
       DigitalOut. This always consists of 32-bit samples from/for port0;
       since ports 1 and 2 of my USB6229 do not support timed operation,
       they are not represented here.
  */
public:
  typedef uint32_t DataType;
public:
  DigitalData(int nscans, double fs_hz);
  /*:F DigitalData(int nscans)
   *:D This constructor allocates space for multiple scans of data.
   *:A nscans: number of scans
  */
  ~DigitalData();
  DigitalData(DigitalData const &other);
  DigitalData &operator=(DigitalData const &other);
  void write(QString ofn, QDomElement elt);
  void writeUInt32(QString ofn);
  /*:F writeUInt32
   *:D Writes data to a file in a raw binary format.
   */
  void read(QString ifn, QDomElement elt);
  void readUInt32(QString ifn);
  /*:F readUInt32
   *:D Reads data back from a file previously written by writeUInt32.
   */
  DataType const *allData() const { return data; }
  DataType *allData(KeyAccess::WriteKey *) { return data; }
  /*:F unsigned long *allData()
   *:D Returns a pointer to all data, that is, to the first scan.
   *:R Pointer to data.
   *:N There are getNumScans() longs at the location returned.
  */
  int getNumScans() const { return nscans; }
  /*:F int getNumScans()
   *:D Return the number of scans in this chunk.
   */
  void setSamplingFrequency(double fs_hz);
  double getSamplingFrequency() const { return fs_hz; }
  void setNumScans(int nscans);
  /*:F setNumScans()
   *:D Specifies the actual number of scans in this chunk. This does not
       reallocate memory, and throws an exception if nscans does not fit
       in the current buffer. See reshape.
   */
  bool reshape(int nscans, bool free=false);
  /*:F reshape
   *:D Reshapes the data buffer. This may claim more space if needed; it
       will not release any unneeded space unless FREE is set to true.
   *:A True if reallocation occurred.
   */
  void zero();
  /*:F zero
   *:D Set all data to zero.
   */
  DataType getMask() const { return cmask; }
  void setMask(DataType mask); 
  void clearMask();
  void defineLine(unsigned int line, QString id);
  void addLine(unsigned int line);
  unsigned int addLine();
  bool hasLine(QString id) const;
  bool hasLine(unsigned int line) const;
  unsigned int findLine(QString id) const;
  DataType maskForLine(QString id) const;
  DataType maskForLine(unsigned int line) const;
private:
  void ensureIDs();
private:
  int32_t nscans;
  int32_t nallocated;
  double fs_hz;
  DataType *data;
  DataType cmask;
  QMap<QString, unsigned int> id2line;
  QMap<unsigned int, QString> line2id;
};

#endif
