#pragma once

#include <client/html/html_video_element.hpp>
#include "./html_media_element-inl.hpp"

namespace dombinding
{
  class HTMLVideoElement : public HTMLMediaElementBase<HTMLVideoElement, dom::HTMLVideoElement>
  {
  public:
    using HTMLMediaElementBase::HTMLMediaElementBase;

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLVideoElement>> GetClassProperties(Napi::Env env);
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
