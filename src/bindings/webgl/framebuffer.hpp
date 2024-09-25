#pragma once

#include <napi.h>
#include "./object.hpp"

namespace webgl
{
  class WebGLFramebuffer : public WebGLObjectBase<WebGLFramebuffer>
  {
  public:
    /**
     * Create a new instance of WebGLFramebuffer.
     * 
     * @param env The Napi::Env that is calling this function.
     * @param isHostFramebuffer Whether the framebuffer is a host framebuffer.
     * @return Napi::Object The new instance of WebGLFramebuffer.
     */
    static Napi::Object NewInstance(Napi::Env env, bool isHostFramebuffer = false)
    {
      Napi::EscapableHandleScope scope(env);
      Napi::Object obj = WebGLObjectBase<WebGLFramebuffer>::NewInstance(env);
      WebGLFramebuffer *instance = WebGLFramebuffer::Unwrap(obj);
      instance->isHost = isHostFramebuffer;
      return scope.Escape(obj).ToObject();
    }

  public:
    WebGLFramebuffer(const Napi::CallbackInfo &info);
    static void Init(Napi::Env env);

  public:
    /**
     * Host framebuffers are created and managed by the host process.
     *
     * FIXME: Currently, we only support 1 host framebuffers which is used in the XRWebGLLayer, developers use the host
     * framebuffer to render the XR content to the scene created by the host.
     */
    bool isHost;

  public:
    static Napi::FunctionReference *constructor;
  };
}
