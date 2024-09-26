#pragma once

#include "client/dom/html_video_element.hpp"
#include "./html_media_element-inl.hpp"

namespace dombinding
{
  class HTMLVideoElement : public HTMLMediaElementBase<HTMLVideoElement, dom::HTMLVideoElement>
  {
  public:
    using HTMLMediaElementBase::HTMLMediaElementBase;

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLVideoElement>> GetClassProperties();
    static void Init(Napi::Env env);

  public:
    static Napi::FunctionReference *constructor;
  };
}
