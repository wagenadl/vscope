#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

int main(int, char **) {
  struct timeval tv0;
  struct timeval tv1;
  printf("gettimeofday -> %i\n",gettimeofday(&tv0,0));
  for (int i=0; i<10; i++) {
    sleep(1);
    gettimeofday(&tv1,0);
    double dt = tv1.tv_sec-tv0.tv_sec;
    dt += (tv1.tv_usec-tv0.tv_usec)/1e6;
    printf("dt = %.3f\n",dt);
  }
  return 0;
}


