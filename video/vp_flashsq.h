// vp_flashsq.h

#ifndef VP_FLASHSQ_H

#define VP_FLASHSQ_H

#include <video/vplugin.h>

class VP_FlashSq: public VPlugin {
public:
  VP_FlashSq();
  QString name() { return "FlashSq"; }
  QString parnames() { return "Period:Duty"; }
  QString parunits() { return "s:%"; }
  QString description() { return "Periodic flashes in a square"; }
  QString parsuggest(unsigned int parno);
  bool setpar(unsigned int parno, double value);
  void render(VPRenderInfo const &info) const;
private:
  double period_s;
  double duty_frc;
};

#endif
