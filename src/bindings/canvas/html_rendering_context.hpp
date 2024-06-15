#pragma once

#include <napi.h>
#include "client/per_process.hpp"
#include "./canvas.hpp"

namespace canvasbinding
{
  /**
   * The HTMLRenderingContext class is to render a HTML document into a specific canvas, this is introduced by JSAR and provided to
   * JSAR-DOM project to render the HTML document into a texture via a canvas object.
   * 
   * In the other hand, this is also useful to provide the HTML rendering capability to the developer who wants to customize the HTML
   * rendering process.
   * 
   * Security Note: See https://robert.ocallahan.org/2011/09/risks-of-exposing-web-page-pixel-data.html, this article explains the risk
   * of exposing the pixel data of a web page to the web page itself, so the `HTMLRenderingContext` is designed to be secure and only
   * render the HTML document into a canvas object, and the canvas object is not allowed to access the pixel data once the canvas creates
   * a HTMLRenderingContext object.
   */
  class HTMLRenderingContext : public Napi::ObjectWrap<HTMLRenderingContext>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, OffscreenCanvas *canvas);
    HTMLRenderingContext(const Napi::CallbackInfo &info);
    ~HTMLRenderingContext();

  private:
    Napi::Value SetHTML(const Napi::CallbackInfo &info);
    Napi::Value DispatchEvent(const Napi::CallbackInfo &info);

  private:
    Napi::ObjectReference *jsCanvas = nullptr;
    TrClientContextPerProcess *clientContext = nullptr;
    SkCanvas *skCanvas;

  public:
    static Napi::FunctionReference *constructor;
  };
}
