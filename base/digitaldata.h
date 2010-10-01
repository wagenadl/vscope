// digitalio.h

#ifndef DIGITALDATA_H

#define DIGITALDATA_H

#include <base/exception.h>
#include <base/types.h>

class DigitalData_ {
protected:
  int32_t nscans;
  int32_t nallocated;
  uint32_t *data;
  uint32_t cmask;
};

class DigitalData: private DigitalData_ {
  /*:C DigitalData
   *:D This class holds any amount of analog data from DigitalIn or for
       DigitalOut. This always consists of 32-bit samples from/for port0;
       since ports 1 and 2 of my USB6229 do not support timed operation,
       they are not represented here.
  */
public:
  static const int MAXLINES = 32;
public:
  DigitalData(int nscans) throw(Exception);
  /*:F DigitalData(int nscans)
   *:D This constructor allocates space for multiple scans of data.
   *:A nscans: number of scans
  */
  DigitalData(FILE *in) throw(Exception);
  /*:F DigitalData(FILE *in)
   *:D This constructor reads back data written by write().
   *:A FILE *in: file handle.
   *:N We use file handles rather than file names in order to accommodate
       storage of multiple chunks of data in one file.
    :  Upon return, the file pointer points to immediately after the chunk
       that was read.
   */
  ~DigitalData();
  DigitalData(DigitalData const &other);
  DigitalData &operator=(DigitalData const &other);
  void read(FILE *in) throw(Exception);
  /*:F void read(FILE *in)
   *:D Reads back data written by write().
   *:A FILE *in: File handle from which to read.
   *:N Upon return, the file pointer points to immediately after the chunk
       that was read.
    :  If the data contained in the file has a different scan
       count, this may involve releasing and reallocating memory.
  */   
  int write(FILE *out) throw(Exception);
  /*:F int write(FILE *out, int overwritenscans=0)
   *:D Writes data to a file.
   *:A FILE *out: File handle to write to.
   *:R Number of bytes written to file.
   *:N Upon return, the file pointer points to immediately after the chunk
       that was written.
  */
  void writeUInt32(QString ofn) throw(Exception);
  /*:F writeUInt32
   *:D Writes data to a file in a raw binary format.
   */
  void readUInt32(QString ifn) throw(Exception);
  /*:F readUInt32
   *:D Reads data back from a file previously written by writeUInt32.
   */
  uint32_t *allData() const { return data; }
  /*:F unsigned long *allData()
   *:D Returns a pointer to all data, that is, to the first scan.
   *:R Pointer to data.
   *:N There are getNumScans() longs at the location returned.
  */
  int getNumScans() const { return nscans; }
  /*:F int getNumScans()
   *:D Return the number of scans in this chunk.
   */
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
  uint32_t getMask() const { return cmask; }
  void clearMask();
  void addLine(uint32_t line);
  /*:F getMask, clearMask, addLine
   *:N The mask is not currently used by either DigitalIn or DigitalOut.
  */
private:
  void copy(DigitalData const &other);
};

#endif
