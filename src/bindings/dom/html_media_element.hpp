#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/html_media_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  template <typename ObjectType, typename HTMLMediaElementType>
  class HTMLMediaElementBase : public HTMLElementBase<ObjectType, HTMLMediaElementType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties(Napi::Env env);

  public:
    using HTMLElementBase<ObjectType, HTMLMediaElementType>::HTMLElementBase;
    HTMLMediaElementBase(const Napi::CallbackInfo &info);

  protected:
    Napi::Value AutoPlayGetter(const Napi::CallbackInfo &info);
    void AutoPlaySetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value CurrentSrcGetter(const Napi::CallbackInfo &info);
    Napi::Value CurrentTimeGetter(const Napi::CallbackInfo &info);
    void CurrentTimeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value DurationGetter(const Napi::CallbackInfo &info);
    Napi::Value EndedGetter(const Napi::CallbackInfo &info);
    Napi::Value LoopGetter(const Napi::CallbackInfo &info);
    void LoopSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value SrcGetter(const Napi::CallbackInfo &info);
    void SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value SrcObjectGetter(const Napi::CallbackInfo &info);
    void SrcObjectSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value VolumeGetter(const Napi::CallbackInfo &info);
    void VolumeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  protected:
    Napi::Value CanPlayType(const Napi::CallbackInfo &info);
    Napi::Value CaptureStream(const Napi::CallbackInfo &info);
    Napi::Value FastSeek(const Napi::CallbackInfo &info);
    Napi::Value Load(const Napi::CallbackInfo &info);
    Napi::Value Pause(const Napi::CallbackInfo &info);
    Napi::Value Play(const Napi::CallbackInfo &info);

  private:
    Napi::ObjectReference srcObjectRef;
  };

  class HTMLMediaElement : public HTMLMediaElementBase<HTMLMediaElement, dom::HTMLMediaElement>
  {
  public:
    using HTMLMediaElementBase::HTMLMediaElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
