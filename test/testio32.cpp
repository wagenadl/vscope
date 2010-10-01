#include <stdio.h>

short _stdcall Inp32(short pa);
void _stdcall Out32(short pa, short data);

int main(int, char **) {
  printf("hello world\n");
  short d = Inp32(0x378);
  printf("lpt -> 0x%04x\n",d);
  d = 0xaa;
  Out32(0x378,d);
  printf("lpt <- 0x%04x\n",d);
  return 0;
}

