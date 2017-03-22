// toplevel/globals.h - This file is part of VScope.
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

// globals.h

#ifndef GLOBALS_H

#define GLOBALS_H

#include <QRect>

class Globals {
public:
  static class ParamTree *ptree; 
  static class DataTrove *trove; 

  static class vscopeGui *gui; 
  static class gt_slots *gtslots; 

  static class MainWindow *mainwindow;
  static class QWidget *leftplace;  
  static class QWidget *rightplace; 
  
  static class ROIImages *ccdw;   
  static class VSDTraces *vsdtraces; 
  static class CohMaps *cohmaps; 
  static class CohGraph *cohgraph; 

  static class MGAuto *mgintra; 
  static class MGAuto *mgextra; 
  static class MGAuto *mgstim; 
  
  static class LiveEPhys *liveephys; 
  static class Focus *focus; 
  static class VProjector *vprojector; 
  static class ContAcq *contacq; 

  static class Trial *trial; 
  static class Acquire *acquire; 

  static class SavedSettings *savedSettings; 

  static class Blackout *blackout; 

  static class TimeButton *walltime, *trialelapsed, *exptelapsed;

  static class Scripts *scripts;
  
  static class ExptLog *exptlog;

  static class PanelHistory *panelHistory;

  static QString filePath();
};

#endif
