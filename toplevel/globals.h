// globals.h

#ifndef GLOBALS_H

#define GLOBALS_H

#include <QMap>
#include <QString>

class Globals {
public:
  static class ParamTree *ptree; // defined in main.cpp
  static class DataTrove *trove; // defined in main.cpp

  static class vscopeGui *gui; // defined in main.cpp
  static class gt_slots *gtslots; // defined in main.cpp

  static class QWidget *mainwindow;
  static class QWidget *leftplace;  // defined in main.cpp
  static class QWidget *rightplace; // defined in main.cpp
  
  static QMap<QString, class ROIImage *> ccdw;   // defined in main.cpp
  static class VSDTraces *vsdtraces; // defined in main.cpp
  static class Coherence *coherence; // defined in main.cpp
  static class CohGraph *cohgraph; // defined in main.cpp

  static class MGAuto *mgintra; // defined in main.cpp
  static class MGAuto *mgextra; // defined in main.cpp
  static class MGAuto *mgstim; // defined in main.cpp
  
  static class LiveEPhys *liveephys; // defined in main.cpp
  static class Focus *focus; // defined in main.cpp
  static class VideoGUI *videogui; // defined in main.cpp
  static class ContAcq *contacq; // defined in main.cpp

  static class Trial *trial; // defined in main.cpp
  static class Acquire *acquire; // defined in main.cpp

  static class SavedSettings *savedSettings; // defined in main.cpp

  static class Blackout *blackout; // defined in main.cpp

  static class TimeButton *walltime, *trialelapsed, *exptelapsed;

  static class Scripts *scripts;
  
  static class ExptLog *exptlog;

  static class PanelHistory *panelHistory;
};

#endif
