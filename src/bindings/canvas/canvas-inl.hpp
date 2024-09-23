#pragma once

#include <napi.h>
#include "client/canvas/canvas.hpp"
#include "./canvas.hpp"
#include "./rendering_context2d.hpp"

namespace canvasbinding
{
  template <typename Context2dType, typename CanvasType>
  Napi::Value CanvasWrap<Context2dType, CanvasType>::GetContext(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0 || !info[0].IsString())
      return env.Null();

    auto typeStr = info[0].ToString().Utf8Value();
    canvas::RenderingContextType contextType = canvas::RenderingContextType::Unset;
    if (typeStr == "2d")
      contextType = canvas::RenderingContextType::RenderingContext2D;
    else if (typeStr == "webgl")
      contextType = canvas::RenderingContextType::WebGL;
    else if (typeStr == "webgl2")
      contextType = canvas::RenderingContextType::WebGL2;
    else if (typeStr == "bitmaprenderer")
      contextType = canvas::RenderingContextType::BitmapRenderer;
    else
      return env.Null();

    auto context = canvasImpl()->getContext(contextType);
    if (context != nullptr)
    {
      if (context->contextType == canvas::RenderingContextType::RenderingContext2D)
        return Context2dType::NewInstance(env, context, info.This());
      // TODO: Add support for other context types: webgl, webgl2, bitmaprenderer
    }
    return env.Null();
  }
}
