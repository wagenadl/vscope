// videogui.h

#ifndef VIDEOGUI_H

#define VIDEOGUI_H

#include <QObject>
#include <video/videoprog.h>

class VideoGUI: public QObject {
  Q_OBJECT;
public:
  VideoGUI(VideoProg &vprog);
  virtual ~VideoGUI();
  bool openPage(class xmlGui *gui, class ParamTree *ptree);
  /*:F openPage
   *:D Responds to opening the GUI page by sending parameters if needed.
   *:R True if successful. If not, a GUI warning is emitted and the
       "Enable" button is switched off.
   */
  bool reset(class xmlGui *gui, class ParamTree *ptree);
  /*:F reset
   *:D Rebuilds gui and prog from ptree.
   *:R True if successful. If not, a GUI warning is emitted and the
       "Enable" button is switched off.
   */
  bool changeEnable(class xmlGui *gui, class ParamTree *ptree);
  bool changeProgram(class xmlGui *gui, class ParamTree *ptree);
  bool changeParam(class xmlGui *gui, class ParamTree *ptree, QString parid);
  /*:F changeEnable, changeProgram, changeParam
   *:D Responds to changes of params in the GUI by sending parameters
       as needed.
   *:R True if successful. If not, a GUI warning is emitted and the
       "Enable" button is switched off.
   */
private:
  void ensureGUI(class xmlGui *gui, class ParamTree *ptree);
  /*:F ensureGUI
   *:D If (and only if) the parameter stimVideo/enable is True,
       makes sure that the program names have been read, and that
       param names are shown properly.
   *:N The first time, this is pretty slow, after, this is instantaneous.
  */
  void populateProgNames(class xmlPage *gui);
  /*:F populateProgNames
   *:D Given a pointer to the "Video Stim" gui page, populates the
       program menu with names instead of numbers.
  */
  void populateParNames(class xmlPage *gui, int prog);
  /*:F populateParNames
   *:D Given a pointer to the "Video Stim" gui page, populates the
       parameter buttons with names for a given program.
  */
  void populateParValues(class xmlPage *gui, int prog, int par);
  /*:F populateParValues
   *:D Given a pointer to the "Video Stim" gui page, populates the
       parameter X menu with appropriate choices.
  */
  void failure(class Exception const &e,
	       class xmlGui *gui, class ParamTree *ptree); 
signals:
  void failed(); // emitted by failure().
private:
  VideoProg &vprog;
  bool populatedprognames;
};

#endif
