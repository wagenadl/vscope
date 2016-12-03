// daqBase.h

#ifndef DAQBASE_H

#define DAQBASE_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QStringList>
#include <base/types.h>
#include <base/exception.h>

#define daq_PRODUCT_NUMBER_TO_USE 0x7346 // USB-6229

typedef Exception daqException;
typedef SysExc daqPerror;

extern void daqTry(int errcode, char const *issuer=0,
		   char const *msg=0, char const *aux=0) throw(daqException);
/*:F daqTry(int errcode, char const *issuer=0, char const *msg=0)
 *:D If errcode is non-zero, throws an exception with detailed information
     based on errcode and NIDAQmx's records.
 *:A errcode: a return value from one of NIDAQmx's functions.
 *:N This function is meant to be used like this:

        daqTry(DAQmxSomething(),"Me","NI doesn't like me");

     and reduces the need for ugly "if (r=DAQmx...) throw(...)" code.
*/

class DAQDevice {
public:
  static bool ok(DAQDevice const *);
public:
  DAQDevice(QString id);
  ~DAQDevice();
  void demand();
  QString id();
  QString serialNo();
  QString typeName();
  unsigned int typeCode();
  int nAIChannels();
  int nAOChannels();
  int nDIOLines();
  void setDIOPort(int portno);
  int dioPort();
  bool isValid() const;
private:
  QString id_;
  int dioport;
  int ndiolines;
public:
  static QStringList deviceList();
  static QString search(QString prodtype, QString serno="");
  static QString search(unsigned int prodtype=0, unsigned int serno=0);
  static DAQDevice &find(QString alias=""); // the default device has name "".
  static void addAlias(QString id, QString alias);
private:
  static void initialize();
  static QString devTypeName(QString id);
  static unsigned int devTypeCode(QString id);
  static QString devSerialNo(QString id);
  static QStringList devAIChannels(QString id);
  static QStringList devAOChannels(QString id);
  static QStringList devDILines(QString devid, int portno);
private:
  static QMap<QString, int> aliases;
  static QVector<DAQDevice *> devices;
};

#endif
