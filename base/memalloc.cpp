// memalloc.cpp

#include "memalloc.h"
#include <stdint.h>

MemExc::MemExc(QString issuer):
  Exception(issuer,"Memory allocation failed") {
}

MemFreeExc::MemFreeExc(QString issuer):
  Exception(issuer,"Memory freeing failed") {
}

