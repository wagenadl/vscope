// msgrecvtest.C

#include <video/videoresp.h>
#include <stdio.h>
#include <base/exception.h>
#include <QApplication>

#define DLINE_VideoResp 4 // from connections.xml

int main(int argc, char **argv) {
  QApplication qap(argc,argv);
  try { while(true) {
    fprintf(stderr,"this is msgrecvtest\n");
    VideoResp vr(DLINE_VideoResp);
    fprintf(stderr,"preparing...\n");
    vr.prepare();
    fprintf(stderr,"waiting...\n");
    unsigned char dest[1024];
    int r;
    int n=0;
    while ((r=vr.retrieveMessage(dest))==0) {
      QApplication::processEvents();
      //      fprintf(stderr,"still waiting %i...\n",++n);
      fflush(stderr);
    }
    if (r<0) {
      fprintf(stderr,"error.\n");
      //return 1;
    } else {
      fprintf(stderr,"received %i bytes: ",r);
      for (n=0; n<r; n++) {
	if (dest[n]>=32 && dest[n]<=126)
	  fprintf(stderr,"%c",dest[n]);
	else
	  fprintf(stderr,"\\%02x",dest[n]);
      }
      fprintf(stderr,"\n");
    }
  }} catch (Exception const &e) {
    e.report();
    return 2;
  }
  return 0;
}

    
