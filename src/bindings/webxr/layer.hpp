#pragma once

#include <napi.h>
#include <client/per_process.hpp>
#include <client/xr/webxr_layers.hpp>
#include <xr/layer.hpp>
#include "../bindings/webgl/rendering_context.hpp"
#include "./common.hpp"
#include "./session.hpp"

namespace bindings
{
  template <typename ObjectType, typename HandleType = client_xr::XRLayer>
  class XRLayerBase : public XRHandleWrap<ObjectType, HandleType>
  {
  public:
    static inline Napi::Object NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle)
    {
      return XRHandleWrap<ObjectType, HandleType>::NewInstance(env, handle);
    }

  public:
    XRLayerBase(const Napi::CallbackInfo &info);

  protected:
    TrClientContextPerProcess *clientContext = nullptr;
  };

  class XRLayer : public XRLayerBase<XRLayer>
  {
  public:
    static void Init(Napi::Env env);
    XRLayer(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRWebGLLayer : public XRLayerBase<XRWebGLLayer, client_xr::XRWebGLLayer>
  {
    friend class XRHandleWrap<XRWebGLLayer, client_xr::XRWebGLLayer>;

  public:
    static void Init(Napi::Env env);
    static Napi::Value NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRWebGLLayer> layer);
    static Napi::Value GetNativeFramebufferScaleFactor(const Napi::CallbackInfo &info);

  public:
    XRWebGLLayer(const Napi::CallbackInfo &info);
    ~XRWebGLLayer();

  public:
    Napi::Value AntialiasGetter(const Napi::CallbackInfo &info);
    Napi::Value IgnoreDepthValuesGetter(const Napi::CallbackInfo &info);
    Napi::Value MultiviewRequiredGetter(const Napi::CallbackInfo &info);
    Napi::Value FramebufferGetter(const Napi::CallbackInfo &info);
    Napi::Value FramebufferWidthGetter(const Napi::CallbackInfo &info);
    Napi::Value FramebufferHeightGetter(const Napi::CallbackInfo &info);
    Napi::Value FixedFoveationGetter(const Napi::CallbackInfo &info);
    void FixedFoveationSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetViewport(const Napi::CallbackInfo &info);

  public:
    XRSession *session;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
