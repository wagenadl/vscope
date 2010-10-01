// pathbar.h

#ifndef PATHBAR_H

#define PATHBAR_H

#include <QVector>
#include <QScrollArea>

class PathBar: public QScrollArea {
  Q_OBJECT;
public:
  PathBar(QWidget *parent);
  virtual ~PathBar();
  void setPath(QString path);
signals:
  void pathSelected(QString path);
protected:
  virtual void resizeEvent(class QResizeEvent *);
  virtual void paintEvent(class QPaintEvent *);
protected slots:
  void elementClicked(QString id);
private:
  void validateArea();
private:
  QWidget *area;
  class QHBoxLayout *layout;
  QVector<class Button *> elements;
  bool asizeValid;
};

#endif
