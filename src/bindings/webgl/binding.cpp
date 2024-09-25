#include "./binding.hpp"
#include "./framebuffer.hpp"
#include "./object.hpp"
#include "./active_info.hpp"
#include "./placeholders.hpp"
#include "./program.hpp"
#include "./rendering_context.hpp"
#include "./texture.hpp"
#include "./uniform_location.hpp"

namespace bindings
{
  namespace webgl
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      ::webgl::WebGLFramebuffer::Init(env);
      ::webgl::WebGLObject::Init(env);
      ::webgl::WebGLActiveInfo::Init(env);
      ::webgl::WebGLProgram::Init(env);
      ::webgl::WebGLTexture::Init(env);
      ::webgl::WebGLUniformLocation::Init(env);
      ::webgl::WebGLRenderingContext::Init(env, exports);
      ::webgl::WebGL2RenderingContext::Init(env, exports);
      InitPlaceholders(env, exports);
      return exports;
    }
  } // namespace webgl
} // namespace bindings
