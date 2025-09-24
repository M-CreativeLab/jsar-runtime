#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl2_rendering_context.hpp>
#include "./webgl_rendering_context.hpp"

namespace script_bindings
{
  class WebGL2RenderingContext;
  using WebGL2RenderingContextBase = scripting_base::ObjectWrap<WebGL2RenderingContext, webgl::WebGL2RenderingContext, WebGLRenderingContext>;

  /**
   * WebGL2RenderingContext wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps webgl::WebGL2RenderingContext objects for use in V8 JavaScript execution contexts.
   * It provides the WebGL 2.0 API extending the base WebGL 1.0 functionality.
   */
  class WebGL2RenderingContext : public WebGL2RenderingContextBase
  {
    using WebGL2RenderingContextBase::ObjectWrap;

  public:
    /**
     * The name of the WebGL2RenderingContext class for V8.
     */
    static std::string Name();

    /**
     * Configure the V8 function template with WebGL2RenderingContext properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 WebGL2RenderingContext instance from a native webgl::WebGL2RenderingContext.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGL2RenderingContext> nativeContext);

    /**
     * Initialize the WebGL2RenderingContext class and register it with V8.
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

  public:
    WebGL2RenderingContext(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // WebGL 2.0 specific methods
    static void DrawBuffers(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ClearBufferfv(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ClearBufferiv(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ClearBufferuiv(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ClearBufferfi(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Query objects
    static void CreateQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void DeleteQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void BeginQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void EndQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetQuery(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetQueryParameter(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Sampler objects
    static void CreateSampler(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void DeleteSampler(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void BindSampler(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void SamplerParameteri(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void SamplerParameterf(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetSamplerParameter(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Sync objects
    static void FenceSync(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void IsSync(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void DeleteSync(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ClientWaitSync(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void WaitSync(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetSyncParameter(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Transform feedback
    static void CreateTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void DeleteTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void BindTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void BeginTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void EndTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void TransformFeedbackVaryings(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetTransformFeedbackVarying(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void PauseTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ResumeTransformFeedback(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Uniform buffer objects
    static void BindBufferBase(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void BindBufferRange(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetUniformIndices(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetActiveUniforms(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetUniformBlockIndex(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetActiveUniformBlockParameter(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetActiveUniformBlockName(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void UniformBlockBinding(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Vertex array objects
    static void CreateVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void DeleteVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void BindVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void IsVertexArray(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Enhanced texture operations
    static void TexImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void TexSubImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void CopyTexSubImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void CompressedTexImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void CompressedTexSubImage3D(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    // Enhanced drawing operations
    static void DrawRangeElements(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void DrawElementsInstanced(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void DrawArraysInstanced(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void VertexAttribDivisor(const v8::FunctionCallbackInfo<v8::Value> &args);
  };
}