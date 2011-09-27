// taperbank.h

#ifndef TAPERBANK_H

#define TAPERBANK_H

#include <math/taperid.h>
#include <math/tapers.h>
#include <QString>
#include <QHash>
#include <QObject>

class Taperbank: public QObject {
  /*:C Taperbank
   *:D Maintains a collection of dpss tapers in memory and on disk
   */
  Q_OBJECT;
public:
  Taperbank(QString dir);
  /*:F Constructor
   *:D Doesn't do anything.
   */
  virtual ~Taperbank();
  void forget(TaperID const &id);
  /*:F forget
   *:D If the named taper exists in memory, drop it.
   *:N This does not affect disk files.
   */
  void destroy(TaperID const &id);
  /*:F destroy
   *:D If the named taper exists in memory, drop it.
       In addition, delete the corresponding file from disk.
  */
  bool contains(TaperID const &id);
  /*:F contains
   *:D Returns true if the named taper exists in memory or on disk.
   */
  bool canProvide(TaperID const &id);
  /*:F canProvide
   *:D Returns true if the named taper exists in memory or on disk, or if
       we believe we are able to create it.
   *:N This is a heuristic, based on the availability of the mkdpss program.
       canProvide does not guarantee that a subsequent call to find() will
       actually succeed. After all, mkdpss can fail.
   */
  bool couldExist(TaperID const &id);
  /*:F couldExist
   *:D Returns true iff the taper is legal, i.e., if K>0.
   */
  Tapers const &find(TaperID const &id);
  /*:F find
   *:D Returns a taper for a given id, or throws an exception if the taper
       couldn't be loaded or constructed.
  */
  void prepare(TaperID const &id);
  /*:F prepare
   *:D Requests that the named taper will be made available. This will
       result in the emission of a prepared() signal, even if the taper
       already exists in memory.
   *:N Throws exception if mkdpss cannot be started.
   */
signals:
  void prepared(TaperID const &id, bool success);
  /*:S prepared
   *:D Emitted when mkdpss completes.
   *:N Also emitted when mkdpss could not be run.
   */
public:
  static void initialize(QString dir);
  /*:F initialize
   *:D Initializes a default taper bank.
   */
  static Taperbank &bank();
  /*:F bank
   *:D Returns a pointer to the default taper bank.
   *:N Throws exception if the bank has not been initialized.
   */
private:
  QString buildfn(TaperID const &id) const;
  void instaPrep(TaperID const &id);
  /*:F instaPrep
   *:D Immediately prepares the named taper.
   *:N Throws exception if it cannot be done.
   */
private:
  QHash<TaperID, Tapers> data;
  QString dir;
  static Taperbank *deflt;
  QHash<TaperID, class QProcess *> preps;
  QHash<QString, TaperID> prepids;
  class QSignalMapper *mapper;
private slots:
  void processDone(QString);
};

#endif
