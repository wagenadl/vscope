// videoresp.h

#ifndef VIDEORESP_H

#define VIDEORESP_H

#include <QObject>
// #include <daq/analogin.h>
// #include <daq/digitalin.h>
#include <base/types.h>

class VideoResp: public QObject {
  Q_OBJECT;
public:
  static const int MAXMSGLEN = 256;
  static const int FREQHZ = 96000;
  static const int CHUNKLEN = 1024;

  static const int STARTSCANS = 7 * 2*FREQHZ/48000; // was:  6
  static const int ZEROSCANS  = 3 * 2*FREQHZ/48000; // was: 15
  static const int ONESCANS   = 5 * 2*FREQHZ/48000; // was: 24
  static const int DANGERSCANS = 100 * FREQHZ/48000;
  static const int TOLERANCE  = 2 * FREQHZ/48000;
  static const int MINSTARTWAKEUPS = 8;
  static const int MININTERWAKEUPS = 2;
public:
  VideoResp(int dline);
  ~VideoResp();
  void prepare();
  /*:F prepare
   *:D Get ready to receive a message
   */
  int retrieveMessage(unsigned char *dest);
  /*:F retrieveMessage
   *:D Retrieves a message, if one has been completely received.
   *:A dest will be filled with the message; it must hold up to MAXMSGLEN-2
       bytes.
   *:R length of message, or 0 if none available, or -ve if error.
   *:N After this, reception is automatically stopped (unless the return
       value is zero). To receive a new message, you must call prepare()
       again.
  */
  void abort();
  /*:F abort
   *:D Call this to give up on receiving a message.
   */
public slots:
  void aDataAv(class AnalogIn *, int nscans);
  void dDataAv(class DigitalIn *, int nscans);
private:
  int dline;
  unsigned char msg[MAXMSGLEN];
  int bit, bytes, wakeups;
  int byte;
  unsigned int checksum;
  bool inmsg, inbyte, inpulse;
  int ilastpulse;
  bool complete;
  bool error;
  bool active;
  // class AnalogData *adata;
  class DigitalData *ddata;
  class AnalogData *adata;
  class AnalogIn *ain;
  class DigitalIn *din;
  uint32_t dmask;
  int ctr_a, ctr_d;
};

#endif
