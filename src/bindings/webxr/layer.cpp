#include "./layer-inl.hpp"
#include "./view.hpp"
#include "./viewport.hpp"
#include "../webgl/framebuffer.hpp"

namespace bindings
{
  using namespace std;

  thread_local Napi::FunctionReference *XRLayer::constructor;
  thread_local Napi::FunctionReference *XRWebGLLayer::constructor;

  void XRLayer::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(env, "XRLayer", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRLayer", tpl);
  }

  XRLayer::XRLayer(const Napi::CallbackInfo &info)
      : XRLayerBase<XRLayer>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Always throw an error if an instance is created directly
    Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
    return;
  }

  void XRWebGLLayer::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(
      env,
      "XRWebGLLayer",
      {InstanceAccessor("antialias", &XRWebGLLayer::AntialiasGetter, nullptr),
       InstanceAccessor("ignoreDepthValues", &XRWebGLLayer::IgnoreDepthValuesGetter, nullptr),
       InstanceAccessor("multiviewRequired", &XRWebGLLayer::MultiviewRequiredGetter, nullptr),
       InstanceAccessor("framebuffer", &XRWebGLLayer::FramebufferGetter, nullptr),
       InstanceAccessor("framebufferWidth", &XRWebGLLayer::FramebufferWidthGetter, nullptr),
       InstanceAccessor("framebufferHeight", &XRWebGLLayer::FramebufferHeightGetter, nullptr),
       InstanceAccessor("fixedFoveation", &XRWebGLLayer::FixedFoveationGetter, &XRWebGLLayer::FixedFoveationSetter),
       InstanceMethod("getViewport", &XRWebGLLayer::GetViewport),
       StaticMethod("getNativeFramebufferScaleFactor", &XRWebGLLayer::GetNativeFramebufferScaleFactor)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRWebGLLayer", tpl);
  }

  Napi::Value XRWebGLLayer::NewInstance(Napi::Env env, shared_ptr<client_xr::XRWebGLLayer> layer)
  {
    assert(layer != nullptr);
    return XRLayerBase<XRWebGLLayer, client_xr::XRWebGLLayer>::NewInstance(env, layer);
  }

  Napi::Value XRWebGLLayer::GetNativeFramebufferScaleFactor(const Napi::CallbackInfo &info)
  {
    // TODO
    return Napi::Number::New(info.Env(), 1.0);
  }

  XRWebGLLayer::XRWebGLLayer(const Napi::CallbackInfo &info)
      : XRLayerBase<XRWebGLLayer, client_xr::XRWebGLLayer>(info)
      , session(nullptr)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (handle_ != nullptr)
    {
      // Skip when the `handle_` is already created.
      return;
    }

    if (info.Length() < 2)
    {
      auto msg = "XRWebGLLayer constructor requires 2 arguments, but only " + std::to_string(info.Length()) + " were provided";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return;
    }

    auto sessionValue = info[0];
    if (!sessionValue.IsObject() || !XRSession::IsInstanceOf(sessionValue))
    {
      Napi::TypeError::New(env, "session must be an `XRSession` object.")
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

    shared_ptr<client_graphics::WebGLContext> glContextObject;
    if (webgl::WebGLRenderingContext::IsInstanceOf(contextValue))
      glContextObject = webgl::WebGLRenderingContext::Unwrap(contextValue.ToObject())->getContext();
    else if (webgl::WebGL2RenderingContext::IsInstanceOf(contextValue))
      glContextObject = webgl::WebGL2RenderingContext::Unwrap(contextValue.ToObject())->getContext();
    else
    {
      auto msg = "the baseLayer must be a WebGLRenderingContext or WebGLRenderingContext2.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return;
    }

    handle_ = client_xr::XRWebGLLayer::Make(session->handle(), glContextObject);

    // Update properties from options
    auto optionsValue = info[2];
    if (optionsValue.IsObject())
    {
      auto optionsObject = optionsValue.As<Napi::Object>();
      if (optionsObject.Has("antialias"))
        handle_->antialias = optionsObject.Get("antialias").ToBoolean().Value();
      if (optionsObject.Has("depth"))
        handle_->depth = optionsObject.Get("depth").ToBoolean().Value();
      if (optionsObject.Has("stencil"))
        handle_->stencil = optionsObject.Get("stencil").ToBoolean().Value();
      if (optionsObject.Has("alpha"))
        handle_->alpha = optionsObject.Get("alpha").ToBoolean().Value();
      if (optionsObject.Has("ignoreDepthValues"))
        handle_->ignoreDepthValues = optionsObject.Get("ignoreDepthValues").ToBoolean().Value();
      if (optionsObject.Has("framebufferScaleFactor"))
        handle_->framebufferScaleFactor = optionsObject.Get("framebufferScaleFactor").ToNumber().FloatValue();
    }
  }

  XRWebGLLayer::~XRWebGLLayer()
  {
    if (!framebufferReference.IsEmpty())
      framebufferReference.Reset();
  }

  Napi::Value XRWebGLLayer::AntialiasGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Boolean::New(info.Env(), handle_->antialias);
  }

  Napi::Value XRWebGLLayer::IgnoreDepthValuesGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Boolean::New(info.Env(), handle_->ignoreDepthValues);
  }

  Napi::Value XRWebGLLayer::MultiviewRequiredGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Boolean::New(info.Env(), handle_->multiviewRequired);
  }

  Napi::Value XRWebGLLayer::FramebufferGetter(const Napi::CallbackInfo &info)
  {
    /**
     * Always returns a host framebuffer because the current layer is to represent the host framebuffer.
     * 
     * TODO: support returning non-host framebuffers.
     */
    if (framebufferReference.IsEmpty())
    {
      auto hostFramebuffer = webgl::WebGLFramebuffer::NewInstance(info.Env(), nullptr, true);
      framebufferReference.Reset(hostFramebuffer, 1);
    }
    return framebufferReference.Value();
  }

  Napi::Value XRWebGLLayer::FramebufferWidthGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), clientContext->getFramebufferWidth());
  }

  Napi::Value XRWebGLLayer::FramebufferHeightGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), clientContext->getFramebufferHeight());
  }

  Napi::Value XRWebGLLayer::FixedFoveationGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), handle_->framebufferScaleFactor);
  }

  void XRWebGLLayer::FixedFoveationSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    handle_->framebufferScaleFactor = value.ToNumber().FloatValue();
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
