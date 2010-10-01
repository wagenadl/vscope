// vp_flashcirc.h

#ifndef VP_FLASHCIRC_H

#define VP_FLASHCIRC_H

#include <video/vplugin.h>

class VP_FlashCirc: public VPlugin {
public:
  VP_FlashCirc();
  QString name() { return "FlashCirc"; }
  QString parnames() { return "Period:Duty"; }
  QString parunits() { return "s:%"; }
  QString description() { return "Periodic flashes in a circle"; }
  QString parsuggest(unsigned int parno);
  bool setpar(unsigned int parno, double value);
  void render(VPRenderInfo const &info) const;
private:
  double period_s;
  double duty_frc;
};

#endif
