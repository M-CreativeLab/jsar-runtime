#pragma once

#include "client/dom/html_image_element.hpp"
#include "./html_element-inl.hpp"
#include "../canvas/image_source.hpp"

namespace dombinding
{
  class HTMLImageElement : public HTMLElementBase<HTMLImageElement, dom::HTMLImageElement>,
                           public canvasbinding::ImageSourceWrap<dom::HTMLImageElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;
    HTMLImageElement(const Napi::CallbackInfo &info);

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLImageElement>> GetClassProperties();
    static void Init(Napi::Env env);
    static Napi::Value ImageConstructor(const Napi::CallbackInfo &info);

  private:
    Napi::Value CurrentSrcGetter(const Napi::CallbackInfo &info);
    Napi::Value SrcGetter(const Napi::CallbackInfo &info);
    void SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value NaturalWidthGetter(const Napi::CallbackInfo &info);
    Napi::Value NaturalHeightGetter(const Napi::CallbackInfo &info);
    Napi::Value WidthGetter(const Napi::CallbackInfo &info);
    Napi::Value HeightGetter(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
