// cache.cpp

#include "cache.h"

Cache::Cache() {
  valid = false;
}

Cache::~Cache() {
}

void Cache::invalidate() {
  valid = false;
}

void Cache::validator() {
  valid = true;
}
