// vp_bars.h

#ifndef VP_BARS_H

#define VP_BARS_H

#include <video/vplugin.h>

class VP_Bars: public VPlugin {
public:
  VP_Bars();
  QString name() { return "Bars"; }
  QString parnames() { return "Temp. Freq.:Spat. Freq.:Direction"; }
  QString parunits() { return "/s:/1:deg"; }
  QString description() { return "Moving bars"; }
  QString parsuggest(unsigned int parno);
  bool setpar(unsigned int parno, double value);
  void render(VPRenderInfo const &info) const;
private:
  double temp_freq_per_s;
  double spat_freq_per_1;
  double direction_rad;
  double starting_phase_1;
};

#endif
