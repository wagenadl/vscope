// gt_slots.h

#ifndef GT_SLOTS_H

#define GT_SLOTS_H

#include <QObject>
#include <QString>

class gt_slots: public QObject {
  Q_OBJECT;
public:
  gt_slots();
  virtual ~gt_slots();
public slots:
  void clicked(QString p);
  void selected(QString p);
  void deselected(QString p);
  void paramchanged(QString p, QString val);
  void customchanged(QString p, int cno, QString val);
};

#endif
