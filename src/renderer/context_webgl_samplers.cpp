#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBindSampler(WebGLuint unit, WebGLuint sampler)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteSamplers(WebGLsizei n, const WebGLuint *samplers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenSamplers(WebGLsizei n, WebGLuint *samplers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsSampler(WebGLuint sampler)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint param)
  {
    /* TODO(yorkie): implement */
  }
}
