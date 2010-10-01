// videocmd.h

#ifndef VIDEOCMD_H

#define VIDEOCMD_H

class VideoCmd {
public:
  static const int FREQHZ = 48000;
  static const int STARTSCANS = 7 * FREQHZ/48000;
  static const int ZEROSCANS  = 3 * FREQHZ/48000;
  static const int ONESCANS   = 5 * FREQHZ/48000;
  static const int STARTWAKEUPS = 10;
  static const int INTERWAKEUPS = 4;
  static const int BITMAXSCANS = ONESCANS;
public:
  VideoCmd(int dline);
  ~VideoCmd();
  void sendMessage(unsigned char *msg, int length);
  void createMessage(int length);
  void addByte(unsigned char value);
  void addBytes(unsigned char *values, int cnt);
  void checkAndSend();
private:
  void reset(int nBytes);
  void addChecksum();
  int byteScans();
  int overheadScans();
  void send();
private:
  int dline;
  int nBytes;
  int nscans;
  class AnalogData *adata;
  class DigitalData *ddata;
  int doffset;
  unsigned int checksum;
};

#endif
