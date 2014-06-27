// trialdata.h

#ifndef TRIALDATA_H

#define TRIALDATA_H

#include <QMap>
#include <QVector>
#include <QString>
#include <base/xml.h>
#include <acq/allccdtimingdetail.h>
#include <base/transform.h>
#include <base/keyagg.h>
#include <base/ccddata.h>
#include <base/analogdata.h>
#include <base/digitaldata.h>

class TrialData: public KeyAgg {
  Q_OBJECT;
public:
  TrialData();
  virtual ~TrialData();
  void useThisPTree(class ParamTree const *ptree);
  void cloneThisPTree(class ParamTree const *ptree);
  void prepare();
  void prepareSnapshot();
  virtual void write() const;
  virtual void read(QString dir, QString exptname, QString trialid);
public:
  ParamTree const *paramTree() const { return partree; } // may be 0 if unprep.
  ParamTree *myParamTree() const { return mypartree; } // may be 0 if unprep.
  // Do NOT change it!
  AnalogData const *analogData() const { return adataIn; }
  AnalogData  *analogData()  { return adataIn; }
  DigitalData const *digitalData() const { return ddataIn; }
  DigitalData  *digitalData()  { return ddataIn; }
  AnalogData const *analogStimuli() const { return adataOut; }
  AnalogData  *analogStimuli()  { return adataOut; }
  DigitalData const *digitalStimuli() const { return ddataOut; }
  DigitalData  *digitalStimuli()  { return ddataOut; }
  /* Note that the above non-const returning functions do not need a write
     key. That's because the returned pointers can only be *used* in a non-
     const way after obtaining a write key from the individual data items.
  */
  CCDData const *ccdData(QString camid) const;
  CCDData *ccdData(QString camid);
  /*:F ccdData
   *:D Throw exception if camid not found
   */
  bool haveCCDData(QString camid) const;
  XML const *getXML() const { return xml; }
  XML *getXML() { return xml; } // only useful after read() or write()
  /*:F getXML
   *:D Only useful after read() or write().
   *:N The writable form breaks the KeyAccess convention.
   */
  QStringList const &cameras() const { return camids; }
signals:
  void newCameras();
  /*:S newCameras
   *:D Emitted when the complement of our cameras has changed.
   */
public:
  bool isPrepared() const { return prep; }
  bool isSnap() const { return snap; }
  bool isCCD() const { return do_ccd; }
  bool hasContEPhys() const { return contEphys; }
  bool isEPhys() const { return !snap && !contEphys; }
  QString filePath() const { return fpath; }
  QString exptName() const { return exptname; }
  QString trialID() const { return trialid; }
  AllCCDTimingDetail const &allTiming() const { return timing_; }
  CCDTimingDetail const &timing(QString camid) const;
  Transform const &ccdPlacement(QString camid) const;
  /*:F ccdPlacement
    :N Throws exception if camid not found */
  void refineCCDTiming();
  /*:F refineCCDTiming
   *:D After trial is complete, update the CCD data's t0 and dt based
       on actual frame times.
  */
private:
  QString trialname() const;
  void generalPrep(bool newccds);
  void writeAnalog(QString base) const;
  void writeDigital(QString base) const;
  void writeCCD(QString base) const;
  void readAnalog(XML &myxml, QString base);
  void readDigital(XML &myxml, QString base);
  void readCCD(XML &myxml, QString base);
  void clearAnalog();
  void clearDigital();
  void clearCCD();
  void prepare(bool concams);
  void prepareSnapshot(bool concams);
  void useConnectedCameras();
private:
  // data
  QStringList camids;
  QMap<QString, CCDData *> ccddata;
  QMap<QString, Transform> ccdplace;
  class AnalogData *adataIn, *adataOut;
  class DigitalData *ddataIn, *ddataOut;
  class ParamTree const *partree;
  class ParamTree *mypartree;
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
  AllCCDTimingDetail timing_;
private:
  Transform camPlace(QString camid) const;
  /*:F camPlace
   *:D Calculates camera placement on a canvas from scratch.
   *:N If ptree is non-zero, acqCCD/region and /binning are used.
   *:N If the camera is unknown, a unit transform is returned
   */
private:
  // not implemented
  TrialData(TrialData const &);
  TrialData &operator=(TrialData const &);
};

#endif
