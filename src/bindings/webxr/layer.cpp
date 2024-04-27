#include "layer.hpp"
#include "view.hpp"
#include "viewport.hpp"

namespace bindings
{
  Napi::FunctionReference *XRLayer::constructor;
  Napi::FunctionReference *XRWebGLLayer::constructor;

  template <typename T>
  XRLayerBase<T>::XRLayerBase(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info)
  {
  }

  Napi::Object XRLayer::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRLayer", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRLayer", tpl);
    return exports;
  }

  XRLayer::XRLayer(const Napi::CallbackInfo &info) : XRLayerBase<XRLayer>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  Napi::Object XRWebGLLayer::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRWebGLLayer",
                                     {InstanceAccessor("antialias", &XRWebGLLayer::AntialiasGetter, nullptr),
                                      InstanceAccessor("ignoreDepthValues", &XRWebGLLayer::IgnoreDepthValuesGetter, nullptr),
                                      InstanceAccessor("framebuffer", &XRWebGLLayer::FramebufferGetter, nullptr),
                                      InstanceAccessor("framebufferWidth", &XRWebGLLayer::FramebufferWidthGetter, nullptr),
                                      InstanceAccessor("framebufferHeight", &XRWebGLLayer::FramebufferHeightGetter, nullptr),
                                      InstanceAccessor("fixedFoveation", &XRWebGLLayer::FixedFoveationGetter, &XRWebGLLayer::FixedFoveationSetter),
                                      InstanceMethod("getViewport", &XRWebGLLayer::GetViewport),
                                      StaticMethod("getNativeFramebufferScaleFactor", &XRWebGLLayer::GetNativeFramebufferScaleFactor)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRWebGLLayer", tpl);
    return exports;
  }

  Napi::Value XRWebGLLayer::NewInstance(Napi::Env env, xr::WebGLLayer layer)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    XRWebGLLayer *instance = XRWebGLLayer::Unwrap(obj);
    instance->config = layer;
    return scope.Escape(obj).ToObject();
  }

  Napi::Value XRWebGLLayer::GetNativeFramebufferScaleFactor(const Napi::CallbackInfo &info)
  {
    // TODO
    return Napi::Number::New(info.Env(), 1.0);
  }

  XRWebGLLayer::XRWebGLLayer(const Napi::CallbackInfo &info) : XRLayerBase<XRWebGLLayer>(info),
                                                               session(nullptr)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() <= 2)
    {
      Napi::TypeError::New(env, "XRWebGLLayer constructor requires two arguments").ThrowAsJavaScriptException();
      return;
    }

    auto sessionValue = info[0];
    if (!sessionValue.IsObject())
    {
      Napi::TypeError::New(env, "XRWebGLLayer constructor requires a XRSession object as the first argument")
          .ThrowAsJavaScriptException();
      return;
    }
    session = Napi::ObjectWrap<XRSession>::Unwrap(sessionValue.As<Napi::Object>());

    auto contextValue = info[1];
    if (!contextValue.IsObject())
    {
      Napi::TypeError::New(env, "XRWebGLLayer constructor requires a WebGLRenderingContext or WebGLRenderingContext2 object as the second argument")
          .ThrowAsJavaScriptException();
      return;
    }
    glContext = Napi::Persistent(contextValue.ToObject());

    // Update properties from options
    auto optionsValue = info[2];
    if (optionsValue.IsObject())
    {
      auto optionsObject = optionsValue.As<Napi::Object>();
      if (optionsObject.Has("antialias"))
        config.antialias = optionsObject.Get("antialias").ToBoolean().Value();
      if (optionsObject.Has("depth"))
        config.depth = optionsObject.Get("depth").ToBoolean().Value();
      if (optionsObject.Has("stencil"))
        config.stencil = optionsObject.Get("stencil").ToBoolean().Value();
      if (optionsObject.Has("alpha"))
        config.alpha = optionsObject.Get("alpha").ToBoolean().Value();
      if (optionsObject.Has("multiview"))
        config.multiview = optionsObject.Get("multiview").ToBoolean().Value();
      if (optionsObject.Has("ignoreDepthValues"))
        config.ignoreDepthValues = optionsObject.Get("ignoreDepthValues").ToBoolean().Value();
      if (optionsObject.Has("framebufferScaleFactor"))
        config.framebufferScaleFactor = optionsObject.Get("framebufferScaleFactor").ToNumber().FloatValue();
    }
  }

  XRWebGLLayer::~XRWebGLLayer()
  {
    glContext.Reset();
  }

  Napi::Value XRWebGLLayer::AntialiasGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Boolean::New(info.Env(), config.antialias);
  }

  Napi::Value XRWebGLLayer::IgnoreDepthValuesGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Boolean::New(info.Env(), config.ignoreDepthValues);
  }

  Napi::Value XRWebGLLayer::FramebufferGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.framebuffer);
  }

  Napi::Value XRWebGLLayer::FramebufferWidthGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.framebufferWidth);
  }

  Napi::Value XRWebGLLayer::FramebufferHeightGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.framebufferHeight);
  }

  Napi::Value XRWebGLLayer::FixedFoveationGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), config.framebufferScaleFactor);
  }

  void XRWebGLLayer::FixedFoveationSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    config.framebufferScaleFactor = value.ToNumber().FloatValue();
  }

  Napi::Value XRWebGLLayer::GetViewport(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "getViewport() requires a view index as the first argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto xrView = XRView::Unwrap(info[0].ToObject());
    auto viewport = xrView->getViewport();
    return XRViewport::NewInstance(env, viewport);
  }
}
