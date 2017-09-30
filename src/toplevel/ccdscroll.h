// toplevel/ccdscroll.h - This file is part of VScope.
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

// ccdscroll.h

#ifndef CCDSCROLL_H

#define CCDSCROLL_H

#include <gfx/hscrollbar.h>

class CCDScroll: public HScrollBar {
  Q_OBJECT;
public:
  CCDScroll(QWidget *parent);
  virtual ~CCDScroll();			    
public slots:
  void newData();
protected slots:
  void sliderMove(double t_ms);
private:
  void rerange(double t0, double t1, double ival);
private:
  double t0, t1, ival;
};

#endif
