// types.cpp

#include <base/types.h>
#include <base/exception.h>
#include <stdio.h>
#include <base/dbg.h>

void checkTypes() {
  if (sizeof(int)<4)
    throw Exception("checkTypes","int has fewer than 32 bits.");
  if (sizeof(int8_t)!=1)
    throw Exception("checkTypes","int8 has incorrect size.");
  if (sizeof(int16_t)!=2)
    throw Exception("checkTypes","int16 has incorrect size.");
  if (sizeof(int32_t)!=4)
    throw Exception("checkTypes","int32 has incorrect size.");
  if (sizeof(int64_t)!=8)
    throw Exception("checkTypes","int64 has incorrect size.");
  
  if (sizeof(uint8_t)!=1)
    throw Exception("checkTypes","uint8 has incorrect size.");
  if (sizeof(uint16_t)!=2)
    throw Exception("checkTypes","uint16 has incorrect size.");
  if (sizeof(uint32_t)!=4)
    throw Exception("checkTypes","uint32 has incorrect size.");
  if (sizeof(uint64_t)!=8)
    throw Exception("checkTypes","uint64 has incorrect size.");

  if (sizeof(float)!=4)
    throw Exception("checkTypes","float has incorrect size.");
  if (sizeof(double)!=8)
    throw Exception("checkTypes","double has incorrect size.");
}
