// trialdata.h

#ifndef TRIALDATA_H

#define TRIALDATA_H

#include <QMap>
#include <QVector>
#include <QString>
#include <base/xml.h>
#include <acq/ccdtimingdetail.h>

class TrialData {
public:
  TrialData();
  virtual ~TrialData();
  QString prepare(class ParamTree const *ptree);
  QString prepareSnapshot(class ParamTree const *ptree);
  virtual QString write() const;
  /*:F write
   *:R Actual trial name, which may differ from trialno if there was a
       pre-existing file. When that happens, letters are added to the
       trialno.
  */
  virtual void read(QString dir, QString exptname, QString trialid,
		    class ParamTree *ptree_dest);
  class AnalogData const *analogData() const { return adataIn; }
  class DigitalData const *digitalData() const { return ddataIn; }
  class CCDData const *ccdData(QString camid) const;
  class AnalogData const *analogStimuli() const { return adataOut; }
  class DigitalData const *digitalStimuli() const { return ddataOut; }
  class XML const *getXML() const { return xml; } // only useful after read() or write()
  class AnalogData  *analogData()  { return adataIn; }
  class DigitalData  *digitalData()  { return ddataIn; }
  class CCDData  *ccdData(QString camid) ;
  class AnalogData  *analogStimuli()  { return adataOut; }
  class DigitalData  *digitalStimuli()  { return ddataOut; }
  class XML *getXML() { return xml; } // only useful after read() or write()
  bool isPrepared() const { return prep; }
  bool isSnap() const { return snap; }
  bool isCCD() const { return do_ccd; }
  bool hasContEPhys() const { return contEphys; }
  bool isEPhys() const { return !snap && !contEphys; }
  QString filePath() const { return fpath; }
  QString exptName() const { return exptname; }
  QString trialID() const { return trialid; }
  CCDTimingDetail const &timing() const { return timing_; }
private:
  static QString trialname(class ParamTree const *tree);
  /*:F trialname
   *:D Determine actual trial name to be used to save data. This may differ
       from the trial number in the ptree if a file preexists, in which case
       letters are added to the file name.
  */  
  QString generalPrep(class ParamTree const *ptree);
  /*:F generalPrep
   *:R Actual trial name (as per trialname()).
   */
  void writeAnalog(QString base) const;
  void writeDigital(QString base) const;
  void writeCCD(QString base) const;
  void readAnalog(XML &myxml, QString base);
  void readDigital(XML &myxml, QString base);
  void readCCD(XML &myxml, QString base);
  void readCCDOldStyle(QVector<QString> &camsstored, QDomElement ccd);
  void readCCDNewStyle(QVector<QString> &camsstored, QDomElement ccd);
private:
  // data
  QMap<QString, int> camidx;
  QVector<QString> camids;
  QVector<CCDData *> ccddata;
  class AnalogData *adataIn, *adataOut;
  class DigitalData *ddataIn, *ddataOut;
  // identification
  QString fpath;
  QString exptname;
  QString trialid;
  // status
  bool prep;
  bool snap; // only valid if prepared
  bool do_ccd; // true if ccd acquisition happens in this trial
  bool contEphys; // only valid if prepared, this means contephys is occurring
                  // concurrently with this trial.
  // xml stuff
  class XML *xml;
  CCDTimingDetail timing_;
private:
  // not implemented
  TrialData(TrialData const &);
  TrialData &operator=(TrialData const &);
};

#endif
