// acq/dutycyclelimit.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// dutycyclelimit.h

#ifndef DUTYCYCLELIMIT_H

#define DUTYCYCLELIMIT_H

#define DUTYCYCLE_PERCENT_MIN 1
#define DUTYCYCLE_PERCENT_MAX 99.9

namespace DutyCycle {
  inline bool triggerEach(double dutyCycle_percent) {
    return dutyCycle_percent < DUTYCYCLE_PERCENT_MAX;
  }
  
  inline double clip(double dutyCycle_percent) {
    if (dutyCycle_percent<DUTYCYCLE_PERCENT_MIN)
      return DUTYCYCLE_PERCENT_MIN;
    else if (dutyCycle_percent<DUTYCYCLE_PERCENT_MAX)
      return dutyCycle_percent;
    else
      return 100;
  }
}

#endif
