#include "binding.hpp"
#include "rendering_context.hpp"
#include "placeholders.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "uniform_location.hpp"

namespace bindings
{
  namespace webgl
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
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
