// ccdtrigmode.h

#ifndef CCDTRIGMODE_H

#define CCDTRIGMODE_H

class CCDTrigMode {
public:
  enum V {
    Immediate,
    FirstFrame,
    EachFrame,
    Bulb,
  };
  CCDTrigMode(V x=Immediate);
  operator int() const { return x; }
  char const *toString() const;
private:
  V x;
};


#endif
