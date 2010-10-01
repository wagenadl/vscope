// roiname.h

#ifndef ROINAME_H
#define ROINAME_H

#define ROIName_STRLEN 4

class ROIName {
 public:
  ROIName(int n=0);
  ROIName next() const;
  bool step(); // returns true if successfully stepped
  char const *getName() const { return name; }
 private:
  char name[4];
};

#endif
