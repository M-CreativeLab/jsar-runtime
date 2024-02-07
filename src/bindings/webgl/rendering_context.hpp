#pragma once

#include <napi.h>
#include "renderer/render_api.hpp"

namespace webgl
{
  class WebGLRenderingContext : public Napi::ObjectWrap<WebGLRenderingContext>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WebGLRenderingContext(const Napi::CallbackInfo &info);

  private:
    Napi::Value CreateProgram(const Napi::CallbackInfo &info);
    Napi::Value LinkProgram(const Napi::CallbackInfo &info);
    Napi::Value UseProgram(const Napi::CallbackInfo &info);
    Napi::Value AttachShader(const Napi::CallbackInfo &info);
    Napi::Value DetachShader(const Napi::CallbackInfo &info);
    Napi::Value CreateShader(const Napi::CallbackInfo &info);
    Napi::Value DeleteShader(const Napi::CallbackInfo &info);
    Napi::Value ShaderSource(const Napi::CallbackInfo &info);
    Napi::Value CompileShader(const Napi::CallbackInfo &info);
    Napi::Value CreateBuffer(const Napi::CallbackInfo &info);
    Napi::Value BindBuffer(const Napi::CallbackInfo &info);
    Napi::Value BufferData(const Napi::CallbackInfo &info);
    Napi::Value CreateTexture(const Napi::CallbackInfo &info);
    Napi::Value BindTexture(const Napi::CallbackInfo &info);
    Napi::Value TexImage2D(const Napi::CallbackInfo &info);
    Napi::Value TexParameteri(const Napi::CallbackInfo &info);
    Napi::Value ActiveTexture(const Napi::CallbackInfo &info);
    Napi::Value GenerateMipmap(const Napi::CallbackInfo &info);
    Napi::Value EnableVertexAttribArray(const Napi::CallbackInfo &info);
    Napi::Value VertexAttribPointer(const Napi::CallbackInfo &info);
    Napi::Value GetAttribLocation(const Napi::CallbackInfo &info);
    Napi::Value GetUniformLocation(const Napi::CallbackInfo &info);
    Napi::Value UniformMatrix4fv(const Napi::CallbackInfo &info);
    Napi::Value DrawArrays(const Napi::CallbackInfo &info);
    Napi::Value DrawElements(const Napi::CallbackInfo &info);
    Napi::Value Viewport(const Napi::CallbackInfo &info);
    Napi::Value ClearColor(const Napi::CallbackInfo &info);
    Napi::Value ClearDepth(const Napi::CallbackInfo &info);
    Napi::Value ClearStencil(const Napi::CallbackInfo &info);
    Napi::Value Clear(const Napi::CallbackInfo &info);
    Napi::Value DepthFunc(const Napi::CallbackInfo &info);
    Napi::Value Enable(const Napi::CallbackInfo &info);
    Napi::Value Disable(const Napi::CallbackInfo &info);
    Napi::Value Scissor(const Napi::CallbackInfo &info);

  private:
    Napi::Value DrawingBufferWidthGetter(const Napi::CallbackInfo &info);
    void DrawingBufferWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value DrawingBufferHeightGetter(const Napi::CallbackInfo &info);
    void DrawingBufferHeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    RenderAPI *m_renderAPI;

  private:
    static Napi::FunctionReference *constructor;
  };
}
