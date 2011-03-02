// videocomm.h

#ifndef VIDEOCOMM_H

#define VIDEOCOMM_H

#include <video/videocommplugins.h>
class VideoComm: public VideoCommPlugins {
};

/* Note: we cannot do typedefs, because that gives compiler error for
   declaration "class VideoComm *vidcom" in videoprog.h. */
#endif
