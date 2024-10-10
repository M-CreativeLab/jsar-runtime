#pragma once

#include "client/dom/html_canvas_element.hpp"
#include "client/canvas/canvas.hpp"
#include "./html_element-inl.hpp"
#include "../canvas/canvas.hpp"
#include "../canvas/rendering_context2d.hpp"

namespace dombinding
{
  class HTMLCanvasElement : public HTMLElementBase<HTMLCanvasElement, dom::HTMLCanvasElement>,
                            public canvasbinding::CanvasWrap<canvasbinding::CanvasRenderingContext2D, canvas::Canvas>
  {
  public:
    using HTMLElementBase::HTMLElementBase;
    HTMLCanvasElement(const Napi::CallbackInfo &info);

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLCanvasElement>> GetClassProperties();
    static void Init(Napi::Env env);

  private:
    Napi::Value WidthGetter(const Napi::CallbackInfo &info);
    void WidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value HeightGetter(const Napi::CallbackInfo &info);
    void HeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    Napi::Value ToDataURL(const Napi::CallbackInfo &info);
    Napi::Value ToBlob(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
