#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_media_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLMediaElement;
    using HTMLMediaElementBase = scripting_base::ObjectWrap<HTMLMediaElement,
                                                            dom::HTMLMediaElement,
                                                            HTMLElement>;

    class HTMLMediaElement : public HTMLMediaElementBase
    {
      using HTMLMediaElementBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "HTMLMediaElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLMediaElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void SrcGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void CurrentTimeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void DurationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void PausedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void MutedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void VolumeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ReadyStateGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      void SrcSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void CurrentTimeSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void MutedSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void VolumeSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      void Play(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Pause(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Load(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}