// textentry.h - Popup for entering text

#ifndef Textentry_H
#define Textentry_H

#include <QFrame>
#include <QString>

class TextEntry: public QFrame {
  /*:C TextEntry
   *:D A simple text entry widget for vsdscope.
       In typical use, you create the TextEntry, set its autohide to false,
       then call show(). You then act upon the accept() and cancel() signals
       to close it. In between, you could use the edited() signal to
       provide validation.
   *:N A modal version should be created which replaces the show() function
       and the accept() and cancel() signals by a single call that returns
       true/false for accept/cancel.
   */
  Q_OBJECT;
 public:
  TextEntry(QWidget *parent);
  /*:F constructor
   *:D This creates the text edit widget in the center of the parent widget
       and with default width and height (see textentry.cpp: TE_WIDTH,
       TE_HEIGHT).
  */
  ~TextEntry();
  QString text() const;
  /*:F text
   *:D Returns the current text as edited by the user.
   */
  void showEvent(class QShowEvent *);
  /*:F showEvent
   *:D When the widget gets shown, focus gets transferred to the editable
       text.
  */
  void hideEvent(class QHideEvent *);
  /*:F hideEvent
   *:D When the widget is closed, so is its virtkeyboard.
   */
public slots:
  void setMessage(QString s);
  /*:F setMessage
   *:D Updates the message shown above the editable text.
   */
  void setText(QString s);
  /*:F setText
   *:D Updates the editable text directly.
   */
  void click_ok();
  /*:F click_ok
   *:D Simulates a click on the OK button.
   *:N This may close the widget; see setAutoHide.
   */
  void click_cancel();
  /*:F click_cancel
   *:D Simulates a click on the Cancel button.
   *:N This may close the widget; see setAutoHide.
   */
  void click_clear();
  /*:F click_clear
   *:D Simulates a click on the Clear button.
   */    
  void setAutoHide(bool);
  /*:F setAutoHide
   *:D Determines whether a click on 'OK' or 'Cancel' should automatically
       close this widget.
       The default is to leave the widget open.
  */
  void enable_ok(bool);
  /*:F enable_ok
   *:D Enables or disables the OK button. Default is enabled.
   */
  void syntheticKeyPress(QChar asc);
  /*:F syntheticKeyPress
   *:D Handles a synthetic key press.
   *:N Codes handled are: printable characters 32-126, and
       8: backspace, 10/13: enter, 27: escape, 127: delete.
       Other codes are ignored.
  */
 signals:
  void edited(QString s);
  /*:S edited
   *:D Emitted whenever the text is changed in any way
   *:A s: current text
   */
  void accept(QString s);
  /*:S accept
   *:D Emitted when the user clicks OK or presses Enter.
   *:A s: text as approved by user.
   */
  void cancel();
  /*:S cancel
   *:D Emitted when the user clicks Cancel or presses Escape.
   */
 protected:
  class QLabel *message;
  class LineEdit *contents;
  class QPushButton *clearb, *cancelb, *okb;
 private:
  QString txt;
  bool autohide;
  class VirtKeyboard *kb;
};

#endif


