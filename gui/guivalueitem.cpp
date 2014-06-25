// guivalueitem.cpp

#include "guivalueitem.h"

guiValueItem::guiValueItem(guiPage *parnt, QString id, guiRoot *mastr):
  guiButton(parnt, id, mastr) {
  makeItem();
  setVisualType(VT_VarValue);
}

guiValueItem::~guiValueItem() {
}

void guiValueItem::setAllowMultiple() {
  multiok = true;
}

void guiValueItem::setup(EasyXML doc) {

}

void guiValueItem::representState() {
  guiButton::representState();
  setFrameShape(QFrame::Panel);
  if (readonly) {
    representFlat(p);
    setDisabled(!isSelected);
  } else {
    setFrameShadow(isSelected ? QFrame::Sunken : QFrame::Raised);
    setLineWidth(isSelected ? 2 : 1);
  }
  QPalette p = palette();
  QColor bg = p.color(QPalette::Button);
  p.setColor(QPalette::Window, isSelected ? mixColor(bg, QColor("white")) : bg);
  setPalette(p);
}
