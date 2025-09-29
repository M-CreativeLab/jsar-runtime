#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_element.hpp>
#include <client/html/html_audio_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLAudioElement;
    using HTMLAudioElementBase = scripting_base::ObjectWrap<HTMLAudioElement,
                                                            dom::HTMLAudioElement,
                                                            HTMLElement>;

    /**
     * HTMLAudioElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLAudioElement objects for use in V8 JavaScript execution contexts.
     * It provides the HTML Audio element interface with media functionality.
     */
    class HTMLAudioElement : public HTMLAudioElementBase
    {
      using HTMLAudioElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLAudioElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLAudioElement";
      }

      /**
       * Configure the V8 function template with HTMLAudioElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static void AudioConstructor(const v8::FunctionCallbackInfo<v8::Value> &info);

    public:
      HTMLAudioElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Media control methods (inherited from HTMLMediaElement)
      static void Play(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Pause(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Load(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Property getters/setters
      static void SrcGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void SrcSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void VolumeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void VolumeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void MutedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void MutedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void PausedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void CurrentTimeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void CurrentTimeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void DurationGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}