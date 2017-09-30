// toplevel/cohimages.h - This file is part of VScope.
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

// CohImages.h

#ifndef COHIMAGES_H

#define COHIMAGES_H

#include <gfx/cohmaps.h>

class CohImages: public CohMaps {
  Q_OBJECT;
public:
  CohImages(class CohData *data);
  void setup();
  virtual ~CohImages();
public slots:
  void updateCameras();
private:
  void setCameras(QStringList ids);
private:
  class CohData *data;
};

#endif
