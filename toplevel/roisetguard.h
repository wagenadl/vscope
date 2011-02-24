// roisetguard.h

#ifndef ROISETGUARD_H

#define ROISETGUARD_H

#include <base/roiset.h>
#include <QString>

class ROISetGuard: public ROISet {
  /*:C ROISetGuard
   *:D Wrapper around ROISet that can automatically save
   */
  Q_OBJECT;
public:
  ROISetGuard(QObject *parent=0);
  virtual ~ROISetGuard();
public slots:
  void setfn(QString);
  void save(void);
  void load(QString);
private:
  QString fn;
};

#endif
