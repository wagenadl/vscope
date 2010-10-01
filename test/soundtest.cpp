// soundtest.cpp

#include <QApplication>
#include <phonon/mediasource.h>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

#define WAVSRC "test/piano-beep.wav"

int main(int argc, char **argv) {
  QApplication ap(argc,argv);
  Phonon::MediaSource src(WAVSRC);
  Phonon::MediaObject obj;
  Phonon::AudioOutput out;
  Phonon::Path path = Phonon::createPath(&obj,&out);
 
  obj.enqueue(src);
  ap.exec();
  return 0;
}

