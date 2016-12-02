// roiset.h

#ifndef ROISET_H

#define ROISET_H

#include <QDomElement>
#include <QSet>
#include <QMap>
#include <base/roicoords.h>
#include <base/campair.h>
#include <base/idkeyaccess.h>

class ROISet: public IDKeyAccess {
  /*:C ROISet
   *:D This is the list of ROI definitions. It does not contain data for
       any particular actual image.
  */
  Q_OBJECT;
public:
  ROISet(QObject *parent=0);
  ~ROISet();
  void setDefaultCamPair(CamPair const &campair);
  /*:F setDefaultCamPair
   *:D Used when reading old xml that doesn't specify a pair
   */
  int newROI(CamPair const &campair);
  /*:F newROI
   *:D Creates a new ROI for the given camera pair, and returns the ID.
       Caller probably wants to checkout() the roi right after to define it.
   *:N This does not cause changed() signals to be emitted.
   */
  ROICoords const &get(int id) const; // throws exception if not found
  ROICoords &access(WriteKey *key);
  ROICoords &access(WriteKey *key, int id);
  /*:F access
   *:D Accesses a certain ROI using either a key that checked out that ROI,
       or using a key that checked out all (in which case the ID must be
       supplied explicitly).
   *:N While it would logically make a lot of sense to make each ROI emit
       its own newData() signals, who really wants to connect to every single
       ROI in a set? Therefore, write access to ROIs is regulated at the level
       of the set, and the ROIs themselves are not even QObjects.
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
private:
  QSet<int> allids;
  QMap<int, CamPair> cams;
  QMap<int, ROICoords> map;
  int lastid;
  CamPair dfltcam;
};

#endif
