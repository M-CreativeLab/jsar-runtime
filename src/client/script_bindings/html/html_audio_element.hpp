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
      static std::string Name()
      {
        return "HTMLAudioElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Function> CreateAudioConstructor(v8::Isolate *isolate);

    private:
      static void AudioConstructor(const v8::FunctionCallbackInfo<v8::Value> &info);

    public:
      HTMLAudioElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Media control methods (inherited from HTMLMediaElement)
      void Play(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Pause(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Load(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Property getters/setters
      void SrcGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void SrcSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void VolumeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void VolumeSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void MutedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void MutedSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void PausedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void CurrentTimeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void CurrentTimeSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void DurationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}
