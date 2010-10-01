// myscrollarea.h

#ifndef MYSCROLLAREA_H

#define MYSCROLLAREA_H

#include <QScrollArea>

class MyScrollArea: public QScrollArea {
  Q_OBJECT;
public:
  MyScrollArea(QWidget *parent);
  void resizeEvent(class QResizeEvent *);
};

#endif
