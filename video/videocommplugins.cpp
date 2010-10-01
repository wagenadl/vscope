// videocommplugins.cpp

#include "videocommplugins.h"
#include "vcplugins.h"

#include <QApplication>
#include <QTime>
#include <xml/enumerator.h>
#include <base/exception.h>
#include <base/dbg.h>
#include <xml/paramtree.h>
#include <base/digitaldata.h>
#include <base/analogdata.h>
#include <base/ccdavoid.h>

#define PAR_OUTRATE "acqEphys/acqFreq"
// Alternative: "stimEphys/outrate"


VideoCommPlugins::VideoCommPlugins() {
  makePlugins();
  iprog=progs.begin().key();
}

VideoCommPlugins::~VideoCommPlugins() {
  for (QMap<int, VPlugin *>::iterator i=progs.begin(); i!=progs.end(); ++i)
    delete i.value();
}

void VideoCommPlugins::setProgram(int prog) {
  if (progs.contains(prog))
    iprog = prog;
   else 
    throw Exception("VideoCommPlugins","Bad program number");
}

void VideoCommPlugins::setPar(int id, double val) {
  progs[iprog]->setpar(id, val);
}
			
QString VideoCommPlugins::getProgName(int prog) {
  if (progs.contains(prog))
    return progs[prog]->name();
  else
    return "-";
}

QMap<int,QString> VideoCommPlugins::getAllProgNames() {
  QMap<int,QString> prognames;
  for (QMap<int,VPlugin*>::iterator i=progs.begin(); i!=progs.end(); ++i) 
    prognames[i.key()] = i.value()->name();
  return prognames;
}

QStringList VideoCommPlugins::getParNames(int prog) {
  QString names;
  if (progs.contains(prog))
    names = progs[prog]->parnames();
  return names.split(":");
}

QStringList VideoCommPlugins::getParUnits(int prog) {
  QString units;
  if (progs.contains(prog))
    units = progs[prog]->parunits();
  return units.split(":");
}

QStringList VideoCommPlugins::getParValues(int prog, int par) {
  QString values;
  if (progs.contains(prog))
    values = progs[prog]->parsuggest(par);
  Dbg() << "getParValues: " << prog << " " << par << ": " << values;
  return values.split(":");
}

void VideoCommPlugins::prepStim(ParamTree *ptree,
				CCDAvoid const &ccdavoid,
				AnalogData *adata,
				DigitalData *ddata) const {
  if (!adata)
    throw Exception("VideoCommPlugins","No analog data","prepStim");
  if (!ddata)
    throw Exception("VideoCommPlugins","No digital data","prepStim");
  
  double marginpre_ms = ptree->find("stimVideo/marginPreCCD").toDouble();
  double marginpost_ms = ptree->find("stimVideo/marginPostCCD").toDouble();
  double samplingrate_hz = ptree->find(PAR_OUTRATE).toDouble();
  double tstart_s = ptree->find("stimVideo/delay").toDouble()/1000;
  double tend_s = tstart_s + ptree->find("stimVideo/dur").toDouble()/1000;
  double framerate_hz = ptree->find("stimVideo/frameRate").toDouble();

  // Rendering is mainly up to plugin, but we calculate frame start and end
  // information.
  VPRenderInfo info;
  // Timing part of rendering, applies to analog and digital
  info.nscans = adata->getNumScans();
  info.fs_hz = samplingrate_hz;
  info.i0 = int(tstart_s*samplingrate_hz);
  dbg("vcplugin: i0=%i n=%i fs=%g",info.i0,info.nscans, info.fs_hz);
  if (ptree->find("stimVideo/avoidCCD").toBool()) {
    int avoid_i0 = ccdavoid.start_scans
      - int(marginpre_ms*samplingrate_hz/1000);
    int avoid_n = ccdavoid.actv_scans
      + int((marginpre_ms+marginpost_ms)*samplingrate_hz/1000);
    int avoid_di = ccdavoid.ival_scans;
    dbg("  vcp: avoid_i0=%i avoid_n=%i avoid_di=%i",avoid_i0,avoid_n,avoid_di);
    // find the ccd activity that affects our first pulse:
    while (avoid_i0+avoid_n>info.i0)
      avoid_i0-=avoid_di;
    while (avoid_i0+avoid_n<info.i0)
      avoid_i0+=avoid_di;
    // Now, we have either of three cases:
    // (1) V: _______*^^^|______|^^^|______|^^^|___ ...
    //     C: _____|^^^*______|^^^|______|^^^|_____ ...

    // (2) V: _______*^^^|______|^^^|______|^^^|___ ...
    //     C: ^^|______|^^^*______|^^^|______|^^^|___ ...

    // (3) V: _______*^^^|______|^^^|______|^^^|___ ...
    //     C: _|^^^|______|^^^*______|^^^|___ ...
    // That is, avoid_i0 has been shifted s.t. the end of the avoidable
    // period is not before the start of video, but is before the 2nd video
    // pulse. In case (3), all is well. In cases (1) and (2) we'll shift
    // the video.

    if (info.i0<avoid_i0 + avoid_n) // shift start to avoid ccd acq
      info.i0 = avoid_i0 + avoid_n;
    info.frameival = avoid_di;
    info.nfill = avoid_di - avoid_n;
  } else {
    info.frameival = info.nfill = int(samplingrate_hz/framerate_hz);
  }
  dbg("  vcp: i0=%i fi=%i nf=%i",info.i0,info.frameival,info.nfill);
  int iend = int(tend_s*samplingrate_hz)+1;
  if (iend>info.nscans)
    iend=info.nscans;
  info.nframes = (iend-info.i0) / info.frameival;
  dbg("  vcp: iend=%i nf=%i",iend,info.nframes);

  // Analog part of rendering: control of x and y position
  info.nchans = adata->getNumChannels();
  Enumerator *chans = Enumerator::find("AOCHAN");
  info.destx = adata->channelData(chans->lookup("VidX"));
  info.desty = adata->channelData(chans->lookup("VidY"));
  
  // Digital part of rendering: control of light
  Enumerator *lines = Enumerator::find("DIGILINES");
  int lightline = lines->lookup("VideoLight");
  ddata->addLine(lightline);
  uint32_t one = 1;
  info.dmask_light = one<<lightline;
  info.destd = ddata->allData();

  dbg("videocommplugins: i0=%i nfr=%i nfil=%i friv=%i",
      info.i0,info.nframes,info.nfill,info.frameival);
  dbg("videocommplugins: destx=%p desty=%p nsc=%i nch=%i fs=%g",
      info.destx,info.desty,info.nscans,info.nchans,info.fs_hz);

  progs[iprog]->setscale(ptree->find("stimVideo/xscale").toDouble()/1e3,
			 ptree->find("stimVideo/yscale").toDouble()/1e3);
  progs[iprog]->sethome(ptree->find("stimVideo/xhome").toDouble()/1e3,
			ptree->find("stimVideo/yhome").toDouble()/1e3);
  progs[iprog]->render(info);

  extendLight(info,
	      ptree->find("stimVideo/preIllum").toDouble(),
	      ptree->find("stimVideo/postIllum").toDouble());
}

void VideoCommPlugins::extendLight(class VPRenderInfo const &info,
				   double preIllum_ms,
				   double postIllum_ms) const {
  if (preIllum_ms>0) {
    uint32_t *searchp = info.destd;
    uint32_t *endp = info.destd+info.nscans;
    uint32_t *firstp = info.destd;
    int preScans = int(preIllum_ms*info.fs_hz/1000);
    while (searchp<endp) {
      if (*searchp & info.dmask_light) {
	uint32_t *backtrackp = searchp - preScans;
	if (backtrackp<firstp)
	  backtrackp = firstp;
	while (backtrackp<searchp)
	  *backtrackp++ |= info.dmask_light;
	while (searchp<endp && (*searchp & info.dmask_light))
	  searchp++;
	firstp = searchp;
      } else {
	searchp++;
      }
    }
  }
  
  if (postIllum_ms>0) {
    uint32_t *searchp = info.destd;
    uint32_t *endp = info.destd+info.nscans;
    int postScans = int(postIllum_ms*info.fs_hz/1000);
    uint32_t *lastp = searchp;
    while (searchp<endp) {
      if (*searchp & info.dmask_light) 
	lastp = searchp + postScans;
      else if (searchp<lastp)
	*searchp |= info.dmask_light;
      *searchp++;
    }
  }	  
}
