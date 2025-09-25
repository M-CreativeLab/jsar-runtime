#pragma once

#include <memory>
#include <client/graphics/webgl_context.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLRenderingContext;
    using WebGLRenderingContextBase = scripting_base::ObjectWrap<WebGLRenderingContext,
                                                                 client_graphics::WebGLContext>;

    class WebGLRenderingContext : public WebGLRenderingContextBase
    {
      using WebGLRenderingContextBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLRenderingContext class for V8.
       */
      static std::string Name()
      {
        return "WebGLRenderingContext";
      }

      static void SetupConstants(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLContext> nativeContext);

    public:
      WebGLRenderingContext(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Canvas properties
      static void CanvasGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DrawingBufferWidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DrawingBufferHeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // State management
      static void ActiveTexture(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void AttachShader(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BindAttribLocation(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BindBuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BindFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BindRenderbuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BindTexture(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Buffer operations
      static void BufferData(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BufferSubData(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Framebuffer operations
      static void CheckFramebufferStatus(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Clear(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ClearColor(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ClearDepth(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ClearStencil(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ColorMask(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Shader operations
      static void CompileShader(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CreateProgram(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CreateShader(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Resource creation
      static void CreateBuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CreateFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CreateRenderbuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void CreateTexture(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Resource deletion
      static void DeleteBuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DeleteFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DeleteProgram(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DeleteRenderbuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DeleteShader(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DeleteTexture(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing operations
      static void DrawArrays(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DrawElements(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Enable/disable state
      static void Enable(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Disable(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Vertex attributes
      static void EnableVertexAttribArray(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DisableVertexAttribArray(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Program operations
      static void GetAttribLocation(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetUniformLocation(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void LinkProgram(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void UseProgram(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Shader source
      static void ShaderSource(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Texture operations
      static void TexImage2D(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void TexParameterf(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void TexParameteri(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Uniform operations
      static void Uniform1f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Uniform1i(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Uniform2f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Uniform2i(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Uniform3f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Uniform3i(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Uniform4f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Uniform4i(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void UniformMatrix2fv(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void UniformMatrix3fv(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void UniformMatrix4fv(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Vertex attribute operations
      static void VertexAttrib1f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void VertexAttrib2f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void VertexAttrib3f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void VertexAttrib4f(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void VertexAttribPointer(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Viewport
      static void Viewport(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Error checking
      static void GetError(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Parameter queries
      static void GetParameter(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetProgramParameter(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetShaderParameter(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Info logs
      static void GetProgramInfoLog(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetShaderInfoLog(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Extensions
      static void GetExtension(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetSupportedExtensions(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
