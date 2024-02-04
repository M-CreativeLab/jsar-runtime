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
    Napi::Value Viewport(const Napi::CallbackInfo &info);
    Napi::Value ClearColor(const Napi::CallbackInfo &info);
    Napi::Value ClearDepth(const Napi::CallbackInfo &info);
    Napi::Value ClearStencil(const Napi::CallbackInfo &info);
    Napi::Value Clear(const Napi::CallbackInfo &info);
    Napi::Value Enable(const Napi::CallbackInfo &info);
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
