// videocommplugins.h

#ifndef VIDEOCOMMPLUGINS_H

#define VIDEOCOMMPLUGINS_H

#include <video/videocommbase.h>
#include <QMap>

class VideoCommPlugins: public VideoCommBase {
public:
  VideoCommPlugins();
  virtual ~VideoCommPlugins();
public:
  virtual void setProgram(int prog);
  virtual void setPar(int parno, double val); // parno=1,2,3.
  virtual QMap<int,QString> getAllProgNames();
  virtual QString getProgName(int prog);
  virtual QStringList getParNames(int prog);
  virtual QStringList getParUnits(int prog);
  virtual QStringList getParValues(int prog, int parno); // parno=1,2,3
  virtual void prepStim(class ParamTree /*const*/ *ptree,
			class CCDAvoid const &ccdavoid,
			class AnalogData *adata,
			class DigitalData *ddata) const;
private:
  int iprog;
  QMap<int, class VPlugin *> progs;
private:
  void makePlugins();
  void extendLight(class VPRenderInfo const &info,
		   double preIllum_ms, double postIllum_ms) const;
};

#endif
