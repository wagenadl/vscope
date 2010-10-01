// NIDAQDevs.H

#ifndef NIDAQDEVS_H

#define NIDAQDEVS_H

namespace NIDAQDEVS {

  struct Mapping {
    int id;
    char const *name; //[64];
  };

  extern Mapping productNumbers[];
  extern Mapping productCategories[];

#endif
