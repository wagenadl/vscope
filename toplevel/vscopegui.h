// vscopegui.h

#ifndef VSCOPEGUI_H

#define VSCOPEGUI_H

#include <gui/xmlgui.h>

class vscopeGui: public xmlGui {
  /*:C vscopeGui
   *:D vscope-specific version of xmlGui.
       This class is quite specific to vscope; it knows, for instance,
       which parameters are dependent on each other and what are the
       acceptable combinations.
  */
  Q_OBJECT;
public:
  vscopeGui(class QWidget *parent, class ParamTree *ptree, QDomElement doc);
  virtual ~vscopeGui();
public:
  virtual void setParam(QString path, QString newval);
  /*:F setParam
   *:D Assigns a new value to a parameter, taking care of side effects.
       Several versions exist, to set parameters of various kinds.
   *:N Throws an exception if 'path' does not reference a parameter or
       if the value is invalid (either globally, or in the context of other
       current parameter values).
  */
  virtual bool canSetParam(QString path, QString newval);
  /*:F canSetParam
   *:D Returns true if 'newval' is a valid new value for the parameter
       at path (in the context of other current parameter values).
   *:N Throws an exception if 'path' does not reference a parameter.
   */
};

#endif
