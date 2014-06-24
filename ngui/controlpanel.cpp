// controlpanel.cpp

#include "controlpanel.h"

ControlPanel::ControlPanel(ParamTree *ptree, EasyXML guidef, QWidget *parent):
  guidef(guidef, "controlpanel") {
  resize(guidef.size("width", "height"));
  
virtual ControlPanel::~ControlPanel();
