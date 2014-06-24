// button.h

#ifndef BUTTON_H

#define BUTTON_H

#include <QLabel>
#include <QTime>
#include <QTimer>
#include <base/enums.h>

extern int BUTTON_FontSize;
extern char const *BUTTON_FontFamily;
extern int BUTTON_Width;
extern int BUTTON_Height;
extern char const *BUTTON_Foreground;
extern char const *BUTTON_Background;

class Button: public QLabel {
  /*:C Button
   *:D A simple text button for vsdscope. It can display up to two lines of
       text, which is normally black on a light color. The button can have
       several states:
       - visible/hidden (a hidden button disappears from view entirely)
       - editable/constant (a "constant" button appears flat)
       - selected/not (a selected button has bold-face text)
   *:N The "selected" mechanism relies on Qt's "checked" property. Therefore,
       callers should not directly manipulate the "checked" property.
   *:N The "editable" state is directly equivalent to Qt's "enabled" property,
       and is currently not used anywhere in vscope. (I (DW) seem to have
       opted to use the "enabled" property directly instead.)
   */
  Q_OBJECT;
public:
  Button(QWidget *parent, int lx=0, int ty=0, QString myID="");
  /*:F constructor
   *:D Creates a new button with default properties at a given location.
   *:A lx,ty: top left position within parent.
    :  myID: optional identifying string which will be included in any signals
             this button emits.
  */
  virtual ~Button();
  VISUALTYPE getVisualType() const { return vtype; }
  QString getID() const { return myID; }
  bool getSelected() const { return isSelected; }
  /*:F getSelected
   *:D Returns true if the button is currently selected.
   */
  bool isReadOnly() const { return readonly; }
signals:
  void activated(QString myID, QString text);
  /*:F activated
   *:D Emitted whenever this action button is clicked.
   *:A myID: this button's identifier as defined at construction time.
    :  text: this button's current text.
   *:N This signal is *only* emitted for Action buttons.
   */
  void selected(QString myID, QString text);
  /*:F selected
   *:D Emitted whenever this button becomes selected.
   *:A myID: this button's identifier as defined at construction time.
    :  text: this button's current text.
   *:N This signal is *not* emitted for Action buttons.
  */
  void deselected(QString myID, QString text);
  /*:F selected
   *:D Emitted when this button becomes deselected if it is a Toggle.
   *:A myID: this button's identifier as defined at construction time.
    :  text: this button's current text.
   *:N This signal is *not* emitted for Action buttons.
  */
  void doubleClicked(QString myID, QString text);
  /*:F selected
   *:D Emitted when this button is double clicked.
   *:A myID: this button's identifier as defined at construction time.
    :  text: this button's current text.
  */
  void textChanged(QString myID, QString text);
  /*:F textChanged
   *:D Emitted whenever the text on this button is changed, even in direct
       response to setText.
  */
protected:
  void mousePressEvent(class QMouseEvent *);
  void mouseReleaseEvent(class QMouseEvent *);
  void mouseDoubleClickEvent(class QMouseEvent *);
public slots:
  virtual void makeReadOnly(bool ro);
  virtual void setText(QString txt, bool noemit=false);
  /*:F setText
   *:D Sets the text on the button, as per QAbstractButton, but additionally
       emits the textChanged signal unless the flag is given.
  */
  void makeAction();
  /*:F makeAction
   *:D Make this button into an action button which does not automatically
       become selected or deselected. This is the default.
   *:N Connect to the activated() signal to receive notifications.
   */
  void makeToggle();
  /*:F makeToggle
   *:D Makes this button into a stand-alone button which can be toggled on
       and off by clicking.
   *:N A Toggle button should not be placed in a QButtonGroup.
  */
  void makeRadio();
  /*:F makeRadio
   *:D Makes this button into a radio button, i.e., a button which is
       automatically deselected when one of its family members is selected.
   *:N The family must defined externally using a QButtonGroup.
   */
  void makeItem();
  /*:F makeItem
   *:D A Radio or Toggle button can be an item, which means it will receive
       channel-style decoration if active.
  */
  void setBackground(QColor const &bg);
  /*:F setBackground
   *:D Sets the background color of this button.
   *:A bg: new background color.
   *:N This color is used for selected and non-selected buttons alike, but
       perhaps not for "constant" buttons.
  */
  void setForeground(QColor const &fg);
  /*:F setForeground
   *:D Sets the foreground color of this button.
   *:A fg: new foreground color.
   *:N This color is used irrespective of state.
   */
  // void setEditable(bool edit);
  // /*:F setEditable
  //  *:D Makes this button editable or not. "Editable" does not imply that the
  //      user can directly change the button's text. Rather, it means that
  //      clicking it will do something.
  // */
  virtual void setSelected(bool sel);
  /*:F setSelected
   *:D Makes this button selected or not. "Selected" buttons are rendered
       in bold face and often represent the current value of a multiple-choice
       setting.
   *:N Do not connect this PushButton's toggled() signal to this!
   *:N It is not possible to deselect a radio button.
  */
  void toggleSelected();
  /*:F setSelected
   *:D Makes this button selected if it was previously not selected or
       vice versa. Not recommended for Radio buttons.
   *:N Do not connect this PushButton's toggled() signal to this!
   *:N This calls setSelected.
  */
  void setID(QString newid);
  /*:F setID
   *:D Sets a new ID for this button (to be used in subsequently emitted
       signals).
   */
  void setVisualType(VISUALTYPE vt);
  /*:F setVisualType
   *:N Visual types specify how this button is rendered in its various
       states. They have no bearing on functionality.
  */
private: 
  bool isAction, isToggle, isRadio, isItem;
  bool isEnabled_;
  bool isSelected;
  VISUALTYPE vtype;
  QTime lastClick;
  QTimer clickTimer;
protected:
  QString myID;
  bool readonly;
protected slots:
  void restoreActionFrame();
public:
  void representState();
  /*:F representState
   *:D This will visually update the button to reflect its selected state.
   */
private:
  void representFlat(class QPalette &);
protected:
  void changeEvent(class QEvent *);
  void paintEvent(class QPaintEvent *e);
};

#endif
