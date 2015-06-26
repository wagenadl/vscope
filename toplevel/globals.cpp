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
