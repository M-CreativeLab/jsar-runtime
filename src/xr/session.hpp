#include <stdint.h>

namespace xr
{
  enum XRSessionMode
  {
    IMMERSIVE_VR = 0,
    IMMERSIVE_AR = 1,
    INLINE = 2
  };

  class XRSessionNative
  {
  public:
    XRSessionNative();

  public:
    uint32_t requestAnimationFrame();
    void cancelAnimationFrame(uint32_t handle);
    void end();

  private:
    int session_id_;
    uint32_t depth_data_format_;
    uint32_t depth_usage_;
    uint32_t environment_blend_mode_;
  };
}
