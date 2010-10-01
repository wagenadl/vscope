// dutycyclelimit.h

#ifndef DUTYCYCLELIMIT_H

#define DUTYCYCLELIMIT_H

#define DUTYCYCLE_PERCENT_MIN 5
#define DUTYCYCLE_PERCENT_MAX 95

inline bool dutyCycle_TriggerEach(double dutyCycle_percent) {
  return dutyCycle_percent < DUTYCYCLE_PERCENT_MAX;
}

inline double dutyCycle_Clip(double dutyCycle_percent) {
  if (dutyCycle_percent<DUTYCYCLE_PERCENT_MIN)
    return DUTYCYCLE_PERCENT_MIN;
  else if (dutyCycle_percent<DUTYCYCLE_PERCENT_MAX)
    return dutyCycle_percent;
  else
    return 100;
}

#endif
