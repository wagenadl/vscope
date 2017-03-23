// toplevel/globals.cpp - This file is part of VScope.
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

#include "globals.h"
#include "xml/paramtree.cpp"
#include <gfx/cohmaps.h>

vscopeGui *Globals::gui;
gt_slots *Globals::gtslots;

MainWindow *Globals::mainwindow;
QWidget *Globals::leftplace;
QWidget *Globals::rightplace;

ROIImages *Globals::ccdw;
VSDTraces *Globals::vsdtraces;
CohMaps *Globals::cohmaps;
CohGraph *Globals::cohgraph;

MGAuto *Globals::mgintra;
MGAuto *Globals::mgextra;
MGAuto *Globals::mgstim;

LiveEPhys *Globals::liveephys;
Focus *Globals::focus;

Trial *Globals::trial;
ContAcq *Globals::contacq;
Acquire *Globals::acquire;
VProjector *Globals::vprojector;

SavedSettings *Globals::savedSettings;

TimeButton *Globals::walltime;
TimeButton *Globals::exptelapsed;
TimeButton *Globals::trialelapsed;

Blackout *Globals::blackout;

ExptLog *Globals::exptlog;

ParamTree *Globals::ptree;
DataTrove *Globals::trove;

Scripts *Globals::scripts;

PanelHistory *Globals::panelHistory;

QString Globals::filePath() {
  return ptree->find("filePath").toString();
}
