// scrollbuttonbar.h

#ifndef SCROLLBUTTONBAR_H

#define SCROLLBUTTONBAR_H

#include <QFrame>
#include <QVector>
#include <QColor>
#include <QString>

class ScrollButtonBar: public QFrame {
  /*:C ScrollButtonBar
   *:D A horizontal bar of push buttons that automatically resize to
       fit their text. The buttons are left-aligned in the bar, except
       when they don't fit, in which case they partially scroll off to
       the left.
  */
  Q_OBJECT;
public:
  ScrollButtonBar(QWidget *parent);
  virtual ~ScrollButtonBar();
  void setAllLabels(class QStringList const &labels);
  void setAsPath(QString path); // this currently assumes path start with "/"
  void clear();
  void addButton(QString label);
  void setButtonLabel(int idx, QString label);
  void setButtonBackground(QColor const &bg);
  void setButtonForeground(QColor const &fg);
  QString getAsPath(int idx); 
  /*:F getAsPath
   *:D Returns the current contents of the bar as a path name
   *:N On linux, this ensures that the path starts with a slash. On Windows,
       paths start with drive letters. There is not currently a way to
       change that drive letter.
  */
signals:
  void clicked(int idx, QString txt);
protected:
  virtual void resizeEvent(class QResizeEvent *);
private:
  void relayout();
private slots:
  void recvClick(QString id, QString txt);
  void recvTextChange(QString id, QString txt);
private:
  QVector<class Button *> buttons;
  QColor bg;
  QColor fg;
};

#endif
