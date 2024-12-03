#pragma once

#include <napi.h>
#include "client/graphics/webgl_framebuffer.hpp"
#include "./object.hpp"

namespace webgl
{
  class WebGLFramebuffer : public WebGLObjectBase<WebGLFramebuffer, client_graphics::WebGLFramebuffer>
  {
  public:
    static void Init(Napi::Env env);
    /**
     * A convenient function to check whether a given JavaScript `value` is an instance of `WebGLFramebuffer`.
     *
     * @param value The value to check.
     * @return Whether the value is an instance of `WebGLFramebuffer`.
     */
    static bool IsInstanceOf(const Napi::Value &value)
    {
      return value.As<Napi::Object>().InstanceOf(constructor->Value());
    }
    /**
     * Create a new instance of WebGLFramebuffer.
     *
     * @param env The Napi::Env that is calling this function.
     * @param handle The handle to the client graphics `WebGLFramebuffer`.
     * @param isHostFramebuffer Whether the framebuffer is a host framebuffer.
     * @return Napi::Object The new instance of `WebGLFramebuffer`.
     */
    static Napi::Object NewInstance(
        Napi::Env env,
        std::shared_ptr<client_graphics::WebGLFramebuffer> handle,
        bool isHostFramebuffer = false)
    {
      Napi::EscapableHandleScope scope(env);
      Napi::Object obj;
      if (isHostFramebuffer)
      {
        auto hostHandle = std::make_shared<client_graphics::WebGLHostFramebuffer>();
        obj = WebGLObjectBase<WebGLFramebuffer, client_graphics::WebGLFramebuffer>::NewInstance(env, hostHandle);
        auto instance = WebGLFramebuffer::Unwrap(obj);
        instance->isHost = true;
      }
      else
      {
        obj = WebGLObjectBase<WebGLFramebuffer, client_graphics::WebGLFramebuffer>::NewInstance(env, handle);
      }
      return scope.Escape(obj).ToObject();
    }

  public:
    WebGLFramebuffer(const Napi::CallbackInfo &info)
        : WebGLObjectBase<WebGLFramebuffer, client_graphics::WebGLFramebuffer>(info), isHost(false)
    {
    }

  public:
    /**
     * Host framebuffers are created and managed by the host process.
     *
     * FIXME: Currently, we only support 1 host framebuffers which is used in the XRWebGLLayer, developers use the host
     * framebuffer to render the XR content to the scene created by the host.
     */
    bool isHost;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
