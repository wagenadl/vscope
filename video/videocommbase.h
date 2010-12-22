// videocomm.h

#ifndef VIDEOCOMMBASE_H

#define VIDEOCOMMBASE_H

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMap>

class VideoCommBase {
public:
  VideoCommBase();
  virtual ~VideoCommBase();
public:
  virtual void setProgram(int prog)=0; // progno=1,2,....
  virtual void setPar(int parno, double val)=0; // parno=1,2,3.
  virtual QMap<int,QString> getAllProgNames()=0; // progno=1,2,....
  virtual QString getProgName(int prog)=0;
  virtual QStringList getParNames(int prog)=0;
  virtual QStringList getParUnits(int prog)=0;
  virtual QStringList getParValues(int prog, int parno)=0; // parno=1,2,3
  virtual void prepStim(class ParamTree const *ptree,
			class CCDTiming const &ccdtiming,
			class AnalogData *adata,
			class DigitalData *ddata) const=0;
};

#endif
