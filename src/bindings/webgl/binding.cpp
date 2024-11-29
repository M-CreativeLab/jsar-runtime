#include "./binding.hpp"
#include "./buffer.hpp"
#include "./framebuffer.hpp"
#include "./object.hpp"
#include "./active_info.hpp"
#include "./placeholders.hpp"
#include "./program.hpp"
#include "./rendering_context-inl.hpp"
#include "./renderbuffer.hpp"
#include "./shader.hpp"
#include "./texture.hpp"
#include "./uniform_location.hpp"
#include "./vertex_array.hpp"

namespace bindings
{
  namespace webgl
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      ::webgl::WebGLActiveInfo::Init(env);
      ::webgl::WebGLObject::Init(env);
      ::webgl::WebGLBuffer::Init(env);
      ::webgl::WebGLFramebuffer::Init(env);
      ::webgl::WebGLRenderbuffer::Init(env);
      ::webgl::WebGLProgram::Init(env);
      ::webgl::WebGLTexture::Init(env);
      ::webgl::WebGLShader::Init(env);
      ::webgl::WebGLUniformLocation::Init(env);
      ::webgl::WebGLVertexArray::Init(env);
      ::webgl::WebGLRenderingContext::Init(env, exports);
      ::webgl::WebGL2RenderingContext::Init(env, exports);
      InitPlaceholders(env, exports);
      return exports;
    }
  } // namespace webgl
} // namespace bindings
