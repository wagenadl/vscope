// videocommaudio.h

#ifndef VIDEOCOMMAUDIO_H

#define VIDEOCOMMAUDIO_H

#include <video/videocommbase.h>

class VideoCommAudio: public VideoCommBase {
public:
  VideoCommAudio();
  virtual ~VideoCommAudio();
public:
  virtual void setProgram(int prog);
  virtual void setPar(int parno, double val); // parno=1,2,3.
  virtual QMap<int,QString> getAllProgNames();
  virtual QString getProgName(int prog);
  virtual QStringList getParNames(int prog);
  virtual QStringList getParUnits(int prog);
  virtual QStringList getParValues(int prog, int parno); // parno=1,2,3
  virtual void prepStim(class ParamTree const *ptree,
			class CCDTiming const &ccdtiming,
			class AnalogData *adata,
			class DigitalData *ddata) const;
private:
  QByteArray waitForMessage(int max_ms = 1000);
private:
  class VideoCmd *vidcmd;
  class VideoResp *vidresp;
};

#endif
