// vscopegui.cpp

#include <toplevel/vscopegui.h>
#include <xml/paramtree.h>
#include <base/exception.h>

vscopeGui::vscopeGui(class QWidget *parent, class ParamTree *ptree, QDomElement doc):
  guiRoot(parent,ptree,doc) {
}

vscopeGui::~vscopeGui() {
}

void vscopeGui::setParam(QString path, QString newval) {
  guiRoot::setParam(path,newval);

  // take care of side effects here:
  // ...
}

bool vscopeGui::canSetParam(QString path, QString newval) {
  if (!guiRoot::canSetParam(path,newval))
    return false;

  // check if this value is OK in combination with others:
  // ...
  return true;
}
