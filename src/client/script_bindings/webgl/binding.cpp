#include "binding.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "active_info.hpp"
#include "framebuffer.hpp"
#include "object.hpp"
#include "placeholders.hpp"
#include "program.hpp"
#include "renderbuffer.hpp"
#include "texture.hpp"
#include "uniform_location.hpp"
#include "vertex_array.hpp"
#include "webgl_rendering_context.hpp"

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void Initialize(Local<Context> context)
    {
      Isolate *isolate = context->GetIsolate();
      Local<Object> global = context->Global();

      // Initialize all WebGL API classes
      Local<Function> webglBufferConstructor = WebGLBuffer::Initialize(isolate);
      Local<Function> webglShaderConstructor = WebGLShader::Initialize(isolate);
      Local<Function> webglActiveInfoConstructor = WebGLActiveInfo::Initialize(isolate);
      Local<Function> webglFramebufferConstructor = WebGLFramebuffer::Initialize(isolate);
      Local<Function> webglObjectConstructor = WebGLObject::Initialize(isolate);
      Local<Function> webglPlaceholdersConstructor = WebGLPlaceholders::Initialize(isolate);
      Local<Function> webglProgramConstructor = WebGLProgram::Initialize(isolate);
      Local<Function> webglRenderbufferConstructor = WebGLRenderbuffer::Initialize(isolate);
      Local<Function> webglTextureConstructor = WebGLTexture::Initialize(isolate);
      Local<Function> webglUniformLocationConstructor = WebGLUniformLocation::Initialize(isolate);
      Local<Function> webglVertexArrayConstructor = WebGLVertexArray::Initialize(isolate);

      // Note: WebGL objects are typically not exposed directly to global scope
      // They are created and returned by WebGLRenderingContext methods
      // The constructors are registered internally for object creation

      // All WebGL object classes are now initialized and ready for use by WebGLRenderingContext
    }
  }
}