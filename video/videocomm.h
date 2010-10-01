// videocomm.h

#ifndef VIDEOCOMM_H

#define VIDEOCOMM_H


// #define VIDEOCOMMAUDIO 1
#define VIDEOCOMMPLUGINS 1

#if VIDEOCOMMAUDIO
#include <video/videocommaudio.h>
class VideoComm: public VideoCommAudio {};
#endif

#if VIDEOCOMMPLUGINS
#include <video/videocommplugins.h>
class VideoComm: public VideoCommPlugins {};
#endif

/* Note: we cannot do typedefs, because that gives compiler error for
   declaration "class VideoComm *vidcom" in videoprog.h. */
#endif
