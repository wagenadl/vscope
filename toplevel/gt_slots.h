// gt_slots.h

#ifndef GT_SLOTS_H

#define GT_SLOTS_H

#include <QObject>
#include <QString>

class gt_slots: public QObject {
  Q_OBJECT;
public:
  gt_slots(class guiRoot *master);
  virtual ~gt_slots();
public slots:
  void clicked(QString p);
  void doubleClicked(QString p);
  void selected(QString p);
  void deselected(QString p);
  void paramchanged(QString p, QString val);
  void customchanged(QString p, int cno, QString val);
  void pgopen(QString p, QWidget *w);
  void pgclose(QString p, QWidget *w);
  void everythingChanged();
protected:
  void report(class Exception const &e, QString act);
};

#endif
