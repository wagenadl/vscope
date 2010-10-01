#include <QLabel>

class testLabel: public QLabel {
  Q_OBJECT;
public:
testLabel(QWidget *);
virtual ~testLabel();
public slots:
void foo();
};

