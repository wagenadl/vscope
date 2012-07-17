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
  static class Coherence *coherence; 
  static class CohGraph *cohgraph; 

  static class MGAuto *mgintra; 
  static class MGAuto *mgextra; 
  static class MGAuto *mgstim; 
  
  static class LiveEPhys *liveephys; 
  static class Focus *focus; 
  static class VideoGUI *videogui; 
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
