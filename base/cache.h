// cache.h

#ifndef CACHE_H

#define CACHE_H

class Cache {
public:
  Cache();
  virtual ~Cache();
  void invalidate();
  bool isvalid() const {
    return valid;
  }
  bool validate() {
    if (valid)
      return true;
    validator();
    return valid;
  }
protected:
  virtual void validator(); // the default implementation sets valid=true.
protected:
  bool valid;
};

#endif
