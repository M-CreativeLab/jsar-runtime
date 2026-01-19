#include <renderer/gles/gpu_display_egl.hpp>

using namespace std;
using namespace commandbuffers;

namespace gles
{
  // static
  Ref<DisplayEGL> DisplayEGL::CreateFromCurrent()
  {
    Ref<DisplayEGL> display = AcquireRef(new DisplayEGL(GPUBackendType::kOpenGLES));
    display->initializeFromCurrent();
    return display;
  }

  DisplayEGL::DisplayEGL(GPUBackendType type)
  {
  }

  void DisplayEGL::initializeFromCurrent()
  {
  }
}
