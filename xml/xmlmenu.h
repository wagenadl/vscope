// xmlmenu.h

#ifndef XMLMENU_H

#define XMLMENU_H

#include <xml/setting.h>

#include <QFrame>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QList>
#include <QDomElement>

#define MENU_MarginLX 5
#define MENU_MarginIX 5
#define MENU_MarginTY 20
#define MENU_MarginIY 2
#define MENU_MenuXL 256
#define MENU_MenuYT 512
#define MENU_MenuW  512
#define MENU_MenuH  256

class xmlMenu: public QFrame {
  /* An xmlMenu represents a graphical view onto the current and possible
     values of a parameter.
     An xmlMenu is "populated" based on a <menu> xml element. This means:
     creation of the widget with its various buttons. At this stage, only
     a default value is set.
     The actual value is set once the xmlMenu is "attached" to a <param>
     xml element. Changes here in the menu are then automatically reflected
     in that <param> element until detachment.
     An xmlMenu may optionally be associated with an xmlPage, in which case
     values set in the menu are automatically reflected
............
  */
  Q_OBJECT;
public:
  xmlMenu(QWidget *parent, class xmlPage *page=0);
  ~xmlMenu();
  void populate(QDomElement menu, QDomElement distConfig);
  void attach(QDomElement userConfig, QStringList const &path);
  void detach();
  QString getValueType() const { return valueType; }
  QString getValueSubType() const { return valueEnum; }
  Setting const &getValue() const { return currentValue; }
  QString getValueAsString() const { return currentValue.toString(); }
  void setValue(Setting const &v);
  void setValueAsString(QString s);
  void setCustomValue(int index, Setting const &v);
  void setCustomValueAsString(int index, QString s);
public:
  static void setDefaultItemSize(int itemw, int itemh); // this and foll. in pix.
  static void setDefaultItemMargins(int xomarg, int ximarg, int yomarg, int yimarg);
  static void setDefaultGeometry(int lpix, int tpix, int wpix, int hpix);
  static void setDefaultBackground(class QColor const &color);
  static void setDefaultForeground(class QColor const &color);
signals:
  void valueChanged(Setting const &v);
  void customValueChanged(int index, Setting const &v);
public slots:
  void clickValue(Setting const &v);
  void editCustom(int n);
private:
  void readXML();
  void writeXML();
  QDomElement userConfig;
  QDomElement param;
  bool attached;
protected:
  class xmlPage *parentPage;
  QString myID;
  QString caption;
  QString valueType;
  QString valueEnum;
  Setting currentValue;
  QMap<QString, class Button *> items;
  QMap<int,Setting> customValues;
};

#endif
