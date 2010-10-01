// overridecursor.h

#ifndef OVERRIDECURSOR_H

#define OVERRIDECURSOR_H

#include <QObject>
#include <QCursor>

class OverrideCursor: QObject {
  Q_OBJECT;
public:
  OverrideCursor();
  OverrideCursor(QCursor const &c);
  virtual ~OverrideCursor();
};

#endif
