// roiset.h

#ifndef ROISET_H

#define ROISET_H

#include <QDomElement>
#include <QSet>
#include <QMap>
#include <base/roicoords.h>
#include <QObject>

class CamPair {
public:
  CamPair() { donor=acceptor=""; }
public:
  QString donor;
  QString acceptor;
  bool operator==(CamPair const &other) const {
    return donor==other.donor && acceptor==other.acceptor;
  }
};

class ROISet: public QObject {
  /*:C ROISet
   *:D This is the list of ROI definitions. It does not contain data for
       any particular actual image.
  */
  Q_OBJECT;
public:
  ROISet(QObject *parent=0);
  ~ROISet();
  int newROI(CamPair const &campair);
  /*:F newROI
   *:D Creates a new ROI for the given camera pair, and returns the ID.
       Caller probably wants to checkout() the roi right after to define it.
   *:N This does not cause changed() signals to be emitted.
   */
  ROICoords const &get(int id) const; // throws exception if not found
  ROICoords &checkout(int id); // throws exception if not found
  /*:F checkout
   *:D Checks out one ROI for writing.
   *:N In the current implementation, this doesn't do anything special: there
       is no exclusivity check.
  */
  void checkin(int id);
  /*:F checkin
   *:D Checks a previously checked out ROI back in.
       This causes a changed(id) signal to be emitted.
   *:N There currently is no verification to see if ID was indeed checked out.
   */
  void cancel(int id);
  /*:F cancel
   *:D Checks a previously checked out ROI back in without emitting a
       changed() signal.
  */
  CamPair const &cam(int id) const;
  bool contains(int id) const;
  void remove(int id);
  void write(QDomElement dst) const;
  void save(QString fn) const;
  void read(QDomElement src);
  void load(QString fn);
  QSet<int> const &ids() const;
  QSet<int> idsForCam(CamPair const &cam) const;
  void clear();
signals:
  void changed(int id);
  /*:S changed
   *:D Emitted whenever a single ROI is changed.
   *:N *Not* emitted when all ROIs are changed, e.g., by a read() operation.
       You probably want to connect to the changedAll() signal as well.
  */
  void changedAll();
  /*:S changedAll
   *:D Emitted whenever all ROIs are changed, e.g., by a read() operation.
   *:N *Not* emitted when individual ROIs change, such as at checkin() time.
       You probably want to connect to the changed() signal as well.
  */
private:
  void clearNoSignal();
private:
  QSet<int> allids;
  QMap<int, CamPair> cams;
  QMap<int, ROICoords> map;
  int lastid;
};

class ROIWriter {
  /*:C ROIWriter
   *:D Convenience class to automate checkout/checkin.
   *:E For instance, instead of writing
       { ROICoords &roi = roiset.checkout(id);
         // do whatever with roi
	 roiset.checkin(id);
       }
       you can write
       { ROIWriter wrt(roiset, id);
         // do whatever with wrt.roi
       } 
       That way, your code is exception safe because checkin is automatic at
       block exit.
  */
public:
  ROIWriter(ROISet &rs, int id);
  ~ROIWriter();
  void cancel();
public:
  ROICoords &roi;
private:
  ROISet &rs;
  int id;
  bool canceled;
};

#endif
