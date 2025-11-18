#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/graphics/webgl_context.hpp>
#include "./webgl_rendering_context.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      class WebGL2RenderingContext;
      using WebGL2RenderingContextBase = scripting_base::ObjectWrap<WebGL2RenderingContext,
                                                                    client_graphics::WebGL2Context>;

      class WebGL2RenderingContext : public WebGL2RenderingContextBase
      {
        using WebGL2RenderingContextBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "WebGL2RenderingContext";
        }

        static void SetupConstants(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                                 std::shared_ptr<client_graphics::WebGL2Context> nativeContext);

      private:
        // WebGL 2.0 specific methods
        void ReadBuffer(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DrawBuffers(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ClearBufferfv(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ClearBufferiv(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ClearBufferuiv(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ClearBufferfi(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Query objects
        void CreateQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DeleteQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
        void BeginQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
        void EndQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetQueryParameter(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Sampler objects
        void CreateSampler(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DeleteSampler(const v8::FunctionCallbackInfo<v8::Value> &args);
        void BindSampler(const v8::FunctionCallbackInfo<v8::Value> &args);
        void SamplerParameteri(const v8::FunctionCallbackInfo<v8::Value> &args);
        void SamplerParameterf(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetSamplerParameter(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Sync objects
        void FenceSync(const v8::FunctionCallbackInfo<v8::Value> &args);
        void IsSync(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DeleteSync(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ClientWaitSync(const v8::FunctionCallbackInfo<v8::Value> &args);
        void WaitSync(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetSyncParameter(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Transform feedback
        void CreateTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DeleteTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
        void BindTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
        void BeginTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
        void EndTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
        void TransformFeedbackVaryings(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetTransformFeedbackVarying(const v8::FunctionCallbackInfo<v8::Value> &args);
        void PauseTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
        void ResumeTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Buffer objects
        void BindBufferBase(const v8::FunctionCallbackInfo<v8::Value> &args);
        void BindBufferRange(const v8::FunctionCallbackInfo<v8::Value> &args);
        void VertexAttribIPointer(const v8::FunctionCallbackInfo<v8::Value> &args);
        void VertexAttribI4i(const v8::FunctionCallbackInfo<v8::Value> &args);
        void VertexAttribI4ui(const v8::FunctionCallbackInfo<v8::Value> &args);
        void VertexAttribI4iv(const v8::FunctionCallbackInfo<v8::Value> &args);
        void VertexAttribI4uiv(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Uniform objects
        void GetUniformIndices(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetActiveUniforms(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetUniformBlockIndex(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetActiveUniformBlockParameter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void GetActiveUniformBlockName(const v8::FunctionCallbackInfo<v8::Value> &args);
        void UniformBlockBinding(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Vertex array objects
        void CreateVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DeleteVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);
        void BindVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);
        void IsVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Enhanced framebuffer operations
        void BlitFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &args);
        void RenderbufferStorageMultisample(const v8::FunctionCallbackInfo<v8::Value> &args);
        void FramebufferTextureLayer(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Enhanced texture operations
        void TexImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
        void TexSubImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
        void TexStorage2D(const v8::FunctionCallbackInfo<v8::Value> &args);
        void TexStorage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
        void CopyTexSubImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
        void CompressedTexImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
        void CompressedTexSubImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);

        // Enhanced drawing operations
        void DrawRangeElements(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DrawElementsInstanced(const v8::FunctionCallbackInfo<v8::Value> &args);
        void DrawArraysInstanced(const v8::FunctionCallbackInfo<v8::Value> &args);
        void VertexAttribDivisor(const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    }
  }
} // namespace endor
