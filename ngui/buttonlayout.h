// buttonlayout.h

#ifndef BUTTONLAYOUT_H

#define BUTTONLAYOUT_H

class ButtonLayout {
public:
  ButtonLayout(QRect box, double columns, double rows);
  void gotoColumn(double);
  void gotoRow(double y);
  void nextColumn();
  void nextRow();
  QRect widgetRect() const;
private:
  QRect box;
  double ncol, nrow;
  double icol, irow;
};

#endif
