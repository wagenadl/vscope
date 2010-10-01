// blackout.h

#ifndef BLACKOUT_H

#define BLACKOUT_H

#include <QLabel>

class Blackout: public QLabel {
  /*:C Blackout
   *:D Blacks out the entire GUI.
       For use during acquisition.
  */
  Q_OBJECT;
public:
  Blackout(QWidget *parent);
  virtual ~Blackout();
};

#endif
