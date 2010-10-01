// vp_bars.h

#ifndef VP_BAR2_H

#define VP_BAR2_H

#include <video/vplugin.h>

class VP_Bar2: public VPlugin {
public:
  VP_Bar2();
  QString name() { return "Bar2"; }
  QString parnames() { return "Period:Speed:Direction"; }
  QString parunits() { return "s:fields/s:deg"; }
  QString description() { return "Narrow moving bars"; }
  QString parsuggest(unsigned int parno);
  bool setpar(unsigned int parno, double value);
  void render(VPRenderInfo const &info) const;
private:
  double period_s;
  double speed_fields_per_s;
  double direction_rad;
  double t0_s;
  double width_fields;
};

#endif
