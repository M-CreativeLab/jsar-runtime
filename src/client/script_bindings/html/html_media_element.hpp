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
      /**
       * The name of the HTMLMediaElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLMediaElement";
      }

      /**
       * Configure the V8 function template with HTMLMediaElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLMediaElement instance from a native dom::HTMLMediaElement.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::HTMLMediaElement> nativeMediaElement);

      /**
       * Initialize the HTMLMediaElement class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLMediaElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void SrcGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void CurrentTimeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DurationGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void PausedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void MutedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void VolumeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ReadyStateGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      static void SrcSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void CurrentTimeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void MutedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void VolumeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void Play(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Pause(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Load(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}