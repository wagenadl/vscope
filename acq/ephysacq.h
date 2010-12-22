// ephysacq.h

#ifndef EPHYSACQ_H

#define EPHYSACQ_H

#include <QObject>
#include <QMutex>

class EPhysAcq: public QObject {
  Q_OBJECT;
public:
  EPhysAcq();
  virtual ~EPhysAcq();
  bool prepare(class ParamTree const *ptree);
  /*:F prepare
   *:D Prepares DAQ board. You must call setBuffer before calling prepare.
       This reshapes the data buffers.
   *:R Returns true if DAQ board present and operational.
   */
   
  void setBuffer(class AnalogData *adata, class DigitalData *ddata);
  /*:F setBuffer
   *:D We do not do our own memory management. Rather, this function 
       determines where our results go.
  */
  void setDataAvFcn(void (*)(int analogscans, int digitalscans, void *aux),
		    void *aux);
  /*:F setDataAvFcn
   *:D Specify a function to be called whenever data is available from
       a continuous acquisition. The data is placed in the AnalogData and
       DigitalData structures given through setBuffer.
  */
  //void setDataDump(class QFile *ana, class QFile *digi, class XML *log);
  /*:F setAnalogDump
   *:D File in which analog data from continuous acquisition will be dumped.
   *:N Even for cont. acq., buffers must be defined through setBuffer.
   *:N Not yet sure how data will be encoded. 16-bit integers are attractive,
       but scale cannot be known ahead of time, so that's tricky. Perhaps
       the file will have to be structured with occasional scale definition
       blocks. Or, we could write occasional scale changes to an XML file?
       Note that range reduction is not really necessary for my current
       hardware, so once all values have been seen, scale will remain
       constant for the duration of the recording. Therefore, it shouldn't
       be necessary to write infinitely many scale info blocks.
       One obvious place is inside the <analog> element, create a series
       of <scale> elements, each with an attribute "start" that defines
       the scan no where this scale change happens, and <channel> elements
       inside it to define the new scales.
   */
  class AnalogIn *daq() { return ain; }
  bool isActive() { return active; }
public slots:
  void start();
  void abort();
protected slots:
  void ainEnded();
  void ainAvailable();
  void dinAvailable();
signals:
  void ended();
  /*:S ended
   *:D This signal is emitted when acquisition is complete. At this point,
       the data will have been copied to the buffer defined through
       setBuffer().
  */
private:
  bool createDAQ(class ParamTree const *ptree);
private:
  bool active;
  bool prep;
  double trialtime_ms;
  class AnalogData *adata;
  class DigitalData *ddata;
  class AnalogIn *ain;
  class DigitalIn *din;
  class QTimer *timer;
  void (*dataAvFcn)(int analogscans, int digitalscans, void *aux);
  void *dataAvAux;
  QMutex dataAvLock;
};

#endif
