// numbers.h

#ifndef Numbers_H
#define Numbers_H

class Numbers {
public:
  Numbers();
public:
  double pi;
  double nan;
  double inf;
  bool isNaN(double x);
};

extern Numbers numbers;

#endif
