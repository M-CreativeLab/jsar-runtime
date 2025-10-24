#pragma once

#include <memory>
#include <client/canvas/image_source.hpp>
#include <client/graphics/webgl_context.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      class WebGLRenderingContext;
      using WebGLRenderingContextBase = scripting_base::ObjectWrap<WebGLRenderingContext, client_graphics::WebGLContext>;

      class WebGLRenderingContext : public WebGLRenderingContextBase
      {
        using WebGLRenderingContextBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "WebGLRenderingContext";
        }
        static void SetupConstants(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      private:
        // Canvas properties
        void CanvasGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DrawingBufferWidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DrawingBufferHeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

        // Basic methods
        void GetContextAttributes(const v8::FunctionCallbackInfo<v8::Value> &info);
        void IsContextLost(const v8::FunctionCallbackInfo<v8::Value> &info);
        void MakeXRCompatible(const v8::FunctionCallbackInfo<v8::Value> &info);
        void SetDefaultCoordHandedness(const v8::FunctionCallbackInfo<v8::Value> &info);

        // State management
        void Hint(const v8::FunctionCallbackInfo<v8::Value> &info);
        void LineWidth(const v8::FunctionCallbackInfo<v8::Value> &info);
        void PixelStorei(const v8::FunctionCallbackInfo<v8::Value> &info);
        void PolygonOffset(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CullFace(const v8::FunctionCallbackInfo<v8::Value> &info);
        void FrontFace(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ActiveTexture(const v8::FunctionCallbackInfo<v8::Value> &info);
        void AttachShader(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DetachShader(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BindAttribLocation(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BindBuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BindFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BindRenderbuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BindTexture(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Buffer operations
        void BufferData(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BufferSubData(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Framebuffer operations
        void FramebufferRenderbuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void FramebufferTexture2D(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CheckFramebufferStatus(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Clear(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ClearColor(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ClearDepth(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ClearStencil(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ColorMask(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DepthMask(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DepthFunc(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DepthRange(const v8::FunctionCallbackInfo<v8::Value> &info);
        void StencilFunc(const v8::FunctionCallbackInfo<v8::Value> &info);
        void StencilFuncSeparate(const v8::FunctionCallbackInfo<v8::Value> &info);
        void StencilMask(const v8::FunctionCallbackInfo<v8::Value> &info);
        void StencilMaskSeparate(const v8::FunctionCallbackInfo<v8::Value> &info);
        void StencilOp(const v8::FunctionCallbackInfo<v8::Value> &info);
        void StencilOpSeparate(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BlendColor(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BlendEquation(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BlendEquationSeparate(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BlendFunc(const v8::FunctionCallbackInfo<v8::Value> &info);
        void BlendFuncSeparate(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Renderbuffer operations
        void RenderbufferStorage(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Shader operations
        void CompileShader(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateProgram(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateShader(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Resource creation
        void CreateBuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateRenderbuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CreateTexture(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Resource deletion
        void DeleteBuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DeleteFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DeleteProgram(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DeleteRenderbuffer(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DeleteShader(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DeleteTexture(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Drawing operations
        void DrawArrays(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DrawElements(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Enable/disable state
        void Enable(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Disable(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Vertex attributes
        void EnableVertexAttribArray(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DisableVertexAttribArray(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Program operations
        void GetActiveAttrib(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetActiveUniform(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetAttribLocation(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetUniformLocation(const v8::FunctionCallbackInfo<v8::Value> &info);
        void LinkProgram(const v8::FunctionCallbackInfo<v8::Value> &info);
        void UseProgram(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ValidateProgram(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Shader source
        void ShaderSource(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetShaderSource(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Texture operations
        void TexImage2D(const v8::FunctionCallbackInfo<v8::Value> &info);
        void TexSubImage2D(const v8::FunctionCallbackInfo<v8::Value> &info);
        void TexParameterf(const v8::FunctionCallbackInfo<v8::Value> &info);
        void TexParameteri(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CopyTexImage2D(const v8::FunctionCallbackInfo<v8::Value> &info);
        void CopyTexSubImage2D(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GenerateMipmap(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Uniform operations
        void Uniform1f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform1i(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform2f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform2i(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform3f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform3i(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform4f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform4i(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform1fv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform1iv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform2fv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform2iv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform3fv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform3iv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform4fv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Uniform4iv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void UniformMatrix2fv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void UniformMatrix3fv(const v8::FunctionCallbackInfo<v8::Value> &info);
        void UniformMatrix4fv(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Vertex attribute operations
        void VertexAttrib1f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void VertexAttrib2f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void VertexAttrib3f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void VertexAttrib4f(const v8::FunctionCallbackInfo<v8::Value> &info);
        void VertexAttribPointer(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Viewport & scissor
        void Viewport(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Scissor(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Error checking
        void GetError(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Parameter queries
        void GetParameter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetProgramParameter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetShaderParameter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetShaderPrecisionFormat(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Info logs
        void GetProgramInfoLog(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetShaderInfoLog(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Extensions
        void GetExtension(const v8::FunctionCallbackInfo<v8::Value> &info);
        void GetSupportedExtensions(const v8::FunctionCallbackInfo<v8::Value> &info);

      private:
        static std::vector<float> GetFloatValuesFromValue(v8::Isolate *isolate, v8::Local<v8::Value> value);
        static std::vector<int> GetIntValuesFromValue(v8::Isolate *isolate, v8::Local<v8::Value> value);
      };
    }
  }
} // namespace endor
