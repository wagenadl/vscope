// exception.h

#ifndef EXCEPTION_H

#define EXCEPTION_H

#include <QString>

class Exception {
  /*:C Exception
   *:D This is a rather generic class to report exceptions of any kind
       in vsdscope.
  */
public:
  Exception(QString issuer="", QString msg="", QString aux="");
  /*:F Exception(QString issuer="", QString msg, QString aux="")
   *:D Pass to this constructor the class of function in which the error
       occurred and an error message. The optional "aux" message will be
       added to the issuer's name in parentheses.
   *:A QString issuer: the name of the class or function in which the
         error occurred.
    :  QString msg: the message the issuer wants to relay to the user.
    :  QString aux: optional auxiliary identifying information about
         the issuer.
  */
  Exception(class QObject const *issuer, QString msg="", QString aux="");
  Exception(Exception const &);
  virtual ~Exception();
  void addMessage(QString msg);
  /*:F void addMessage(QString msg)
   *:D This tags an additional message to a previously created Exception.
   */
  void report() const;
  /*:F void report()
   *:D This writes the error message (issuer's name and all) to stderr.
   */
  operator QString() const;
  /*:F operator QString()
   *:D This returns the error message (issuer's name and all) as a string.
   */
  static void immediateReport(bool onof);
  /*:F void immediateReport(bool onof)
   *:D By default, Exception's message gets written to stderr as soon as
       the daqException is created. This static function can turn that
       behavior of if you prefer to catch exceptions and produce reports
       downstream.
   *:A bool onof: true to enable reporting at construction time, false to
       disable.
  */
private:
  void construct(QString issuer, QString msg, QString aux);
protected: // read only for descendents
  QString issuer;
  QString message;
  static bool immrep;
  void **backtrace_data;
  int backtrace_count;
};

class SysExc: public Exception {
  /*:C SysExc
   *:D A descendent of Exception that reports operating system errors
       such as file read/write problems.
  */
 public:
  SysExc(QString issuer="", QString msg="", QString aux="");
  /*:F SysExc(QString issuer="", QString msg, QString aux="")
   *:D The constructor calls strerror() to find out what the operating system
       has to say about the problem, and appends the result to the caller-
       supplied error message.
   *:A See 'Exception.'
   */
};

#endif
