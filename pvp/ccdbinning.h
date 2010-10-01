// ccdbinning.h

#ifndef CCDBINNING_H

#define CCDBINNING_H


class CCDBinning {
public:
  CCDBinning();
  CCDBinning(int bin);
  CCDBinning(int sbin, int pbin);
public:
  int sbin;
  int pbin;
};


#endif
