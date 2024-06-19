#pragma once

#include <napi.h>
#include "common.hpp"
#include "session.hpp"
#include "client/per_process.hpp"
#include "bindings/webgl/rendering_context.hpp"
#include "xr/layer.hpp"

namespace bindings
{
  template <typename T>
  class XRLayerBase : public Napi::ObjectWrap<T>
  {
  public:
    XRLayerBase(const Napi::CallbackInfo &info);

  protected:
    TrClientContextPerProcess* clientContext = nullptr;
  };

  class XRLayer : public XRLayerBase<XRLayer>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    XRLayer(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRWebGLLayer : public XRLayerBase<XRWebGLLayer>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value NewInstance(Napi::Env env, xr::WebGLLayer layer);
    static Napi::Value GetNativeFramebufferScaleFactor(const Napi::CallbackInfo &info);
    XRWebGLLayer(const Napi::CallbackInfo &info);
    ~XRWebGLLayer();

  public:
    Napi::Value AntialiasGetter(const Napi::CallbackInfo &info);
    Napi::Value IgnoreDepthValuesGetter(const Napi::CallbackInfo &info);
    Napi::Value FramebufferGetter(const Napi::CallbackInfo &info);
    Napi::Value FramebufferWidthGetter(const Napi::CallbackInfo &info);
    Napi::Value FramebufferHeightGetter(const Napi::CallbackInfo &info);
    Napi::Value FixedFoveationGetter(const Napi::CallbackInfo &info);
    void FixedFoveationSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetViewport(const Napi::CallbackInfo &info);

  public:
    webgl::WebGLRenderingContext *getWebGLRenderingContext();
    webgl::WebGL2RenderingContext *getWebGL2RenderingContext();

  public:
    xr::WebGLLayer config;
    XRSession *session;
    Napi::ObjectReference glContext;

  private:
    static Napi::FunctionReference *constructor;
  };
}
