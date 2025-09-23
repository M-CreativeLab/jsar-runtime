#include "binding.hpp"
#include "buffer.hpp"
#include "shader.hpp"
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

      // Initialize WebGL API classes
      Local<Function> webglBufferConstructor = WebGLBuffer::Initialize(isolate);
      Local<Function> webglShaderConstructor = WebGLShader::Initialize(isolate);

      // Note: WebGL objects are typically not exposed directly to global scope
      // They are created and returned by WebGLRenderingContext methods
      // The constructors are registered internally for object creation

      // TODO: Initialize remaining WebGL classes:
      // - WebGLProgram, WebGLTexture, WebGLFramebuffer, WebGLRenderbuffer
      // - WebGLUniformLocation, WebGLActiveInfo, WebGLVertexArray
      // - Other WebGL objects as needed
    }
  }
}