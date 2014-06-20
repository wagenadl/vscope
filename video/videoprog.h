// videoprog.h

#ifndef VIDEOPROG_H

#define VIDEOPROG_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QObject>

class VideoProg {
public:
  static const int MAXPAR = 3;
  static VideoProg &find();
public:
  VideoProg();
  virtual ~VideoProg();
  int progname2num(QString name);
  QString prognum2name(int num);
  QMap<int,QString> const &progNames();
  QStringList const &parNames(int prog);
  QStringList const &parUnits(int prog);
  QStringList const &parValues(int prog, int par);
  void prepStim(class ParamTree const *ptree,
		class CCDTiming const &ccdtiming,
		class AnalogData *adata, class DigitalData *ddata) const;
  void reset(class ParamTree /*const*/ *ptree); // ensures our prog and pars match ptree
  bool enabled(class ParamTree *ptree) const;
  bool ok() const;
public: // only meant for class VideoGUI
  void ensureSettings(class ParamTree *ptree);
  /*:F ensureSettings
   *:D If (and only if) the parameter stimVideo/enable is True,
       makes sure that sensible program information has been sent
       to the video computer.
   *:N The first time, this is pretty slow, after, this is instantaneous.
  */
  void setProgram(class ParamTree *ptree, bool forreal);
  /* If forreal is not true, this does not communicate with the hardware.
     ensureSettings() must then be called later.
  */
  void setParam(class ParamTree *ptree, int parno, bool forreal);
private:
  void ensureProgNames();
  void ensureParNames(int prog);
  void ensureParValues(int prog, int par);
private:
  class VideoComm *vidcom;
  bool hasprognames;
  bool isuptodate;
  QMap<QString,int> prognos;
  QMap<int,QString> prognames;
  QMap<int,QStringList> parnames4prog;
  QMap<int,QStringList> parunits4prog;
  QMap<int,QMap<int,QStringList> > parvalues4prog;
  static VideoProg *deflt;
};

#endif
