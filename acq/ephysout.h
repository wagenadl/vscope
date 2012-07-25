// ephysout.h

#ifndef EPHYSOUT_H

#define EPHYSOUT_H

#include <QObject>
#include <acq/allccdtimingdetail.h>
#include <daq/analogout.h>

class EPhysOut: public QObject {
  Q_OBJECT;
public:
  EPhysOut();
  virtual ~EPhysOut();
  bool prepare(class ParamTree const *ptree,
	       AllCCDTimingDetail const &timing); // returns true if DAQ board present and operational
  bool prepareSnap(class ParamTree const *ptree,
		   AllCCDTimingDetail const &timing);
  void setBuffer(class AnalogData *adata, class DigitalData *ddata);
  /*:F setBuffer
   *:D We do not do our own memory management. Rather, this function 
       determines where we prepare our signals.
  */
public slots:
  void commit();
  void setMaster(class EPhysAcq *master);
  void start(); // if we have a master, this only prepares!
  void abort();
protected slots:
  void aoutEnded();
signals:
  void ended();
private:
  void setupDData_addStim(class ParamTree const *ptree);
  void setupAData_stim(class ParamTree const *ptree);
  void setupAData_dummy();
  bool createDAQ(class ParamTree const *ptree); // true iff hardware present
  void setupAData_mkStim(ParamTree const *ptree, QString ch, QString path);
private:
  bool active;
  bool prep;
  double trialtime_ms;
  class AnalogData *adata;
  class DigitalData *ddata;
  class AnalogOut *aout;
  class DigitalOut *dout;
  class QTimer *timer; // for fake "Done" event generation
  class EPhysAcq *master, *oldmaster;
  QList<QString> channelList;
};

#endif
