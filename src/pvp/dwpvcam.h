#include <../pvcam/master.h>
#if defined(__CYGWIN__)
  // master.h defines these for windows and linux but not cygwin
  #define DEPRECATED __declspec(deprecated)
  #define PV_DECL __stdcall
#endif

#include <../pvcam/pvcam.h>

