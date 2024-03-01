#include "session.hpp"

namespace xr
{
  class XRSystemNative
  {
  public:
    static XRSystemNative *GetInstance();
    XRSystemNative();

  public:
    XRSessionNative *getSessionById(int id);
    XRSessionNative *requestSession(XRSessionMode mode);
    void isSessionSupported(XRSessionMode mode);
  };
}
