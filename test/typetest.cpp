#include <base/types.h>

#include <stdio.h>

int main(int argc,  char **argv) {
  int8_t a;
  uint8_t b;
  int16_t c;
  uint16_t d;
  int32_t e;
  uint32_t f;
  int64_t g;
  uint64_t h;
  printf("bytes: %i %i\n",sizeof(a),sizeof(b));
  printf("shorts: %i %i\n",sizeof(c),sizeof(d));
  printf("longs: %i %i\n",sizeof(e),sizeof(f));
  printf("longlongs: %i %i\n",sizeof(g),sizeof(h));
  return 0;
}
