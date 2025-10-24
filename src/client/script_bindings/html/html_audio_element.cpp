#include <iostream>
#include <client/per_process.hpp>
#include <client/dom/browsing_context.hpp>
#include "./html_audio_element.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings::html_bindings
  {
    void HTMLAudioElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Media control methods
      InstanceMethod(isolate, prototype, "play", &HTMLAudioElement::Play);
      InstanceMethod(isolate, prototype, "pause", &HTMLAudioElement::Pause);
      InstanceMethod(isolate, prototype, "load", &HTMLAudioElement::Load);

      // Media properties
      InstanceReadonlyAccessor(isolate, instance, "paused", &HTMLAudioElement::PausedGetter);
      InstanceReadonlyAccessor(isolate, instance, "duration", &HTMLAudioElement::DurationGetter);
      InstanceAccessor(isolate,
                       instance,
                       "src",
                       &HTMLAudioElement::SrcGetter,
                       &HTMLAudioElement::SrcSetter);
      InstanceAccessor(isolate,
                       instance,
                       "volume",
                       &HTMLAudioElement::VolumeGetter,
                       &HTMLAudioElement::VolumeSetter);
      InstanceAccessor(isolate,
                       instance,
                       "muted",
                       &HTMLAudioElement::MutedGetter,
                       &HTMLAudioElement::MutedSetter);
      InstanceAccessor(isolate,
                       instance,
                       "currentTime",
                       &HTMLAudioElement::CurrentTimeGetter,
                       &HTMLAudioElement::CurrentTimeSetter);
    }

    // static
    v8::Local<v8::Function> HTMLAudioElement::CreateAudioConstructor(v8::Isolate *isolate)
    {
      return HTMLElement::CreateElementConstructor(isolate, "Audio", HTMLAudioElement::AudioConstructor);
    }

    // static
    void HTMLAudioElement::AudioConstructor(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (!info.IsConstructCall())
      {
        MakeConstructorError(isolate,
                             "Please use the 'new' operator, this DOM object constructor cannot be called as a function.");
        return;
      }

      auto document = TrClientContextPerProcess::Get()->browsingContext->getActiveDocument();
      auto element = dynamic_pointer_cast<dom::HTMLAudioElement>(document->createElement("audio"));
      if (info.Length() >= 1 && info[0]->IsString())
      {
        String::Utf8Value utf8Value(isolate, info[0]);
        element->setSrc(*utf8Value);
      }

      // Return the wrapped object.
      info.GetReturnValue().Set(NewInstance(isolate, element));
    }

    HTMLAudioElement::HTMLAudioElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLAudioElementBase(isolate, args)
    {
    }

    // Media control methods
    void HTMLAudioElement::Play(const FunctionCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.play() called" << endl;
      // TODO: Implement play functionality - returns Promise
      info.GetReturnValue().SetUndefined();
    }

    void HTMLAudioElement::Pause(const FunctionCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.pause() called" << endl;
      // TODO: Implement pause functionality
      info.GetReturnValue().SetUndefined();
    }

    void HTMLAudioElement::Load(const FunctionCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.load() called" << endl;
      // TODO: Implement load functionality
      info.GetReturnValue().SetUndefined();
    }

    // Property implementations
    void HTMLAudioElement::SrcGetter(const PropertyCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.src getter called" << endl;
      // TODO: Return the current source URL
      info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "").ToLocalChecked());
    }

    void HTMLAudioElement::SrcSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      cout << "HTMLAudioElement.src setter called" << endl;
      // TODO: Set the source URL and trigger loading
    }

    void HTMLAudioElement::VolumeGetter(const PropertyCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.volume getter called" << endl;
      // TODO: Return the current volume (0.0 to 1.0)
      info.GetReturnValue().Set(1.0);
    }

    void HTMLAudioElement::VolumeSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      cout << "HTMLAudioElement.volume setter called" << endl;
      // TODO: Set the volume level
    }

    void HTMLAudioElement::MutedGetter(const PropertyCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.muted getter called" << endl;
      // TODO: Return the muted state
      info.GetReturnValue().Set(false);
    }

    void HTMLAudioElement::MutedSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      cout << "HTMLAudioElement.muted setter called" << endl;
      // TODO: Set the muted state
    }

    void HTMLAudioElement::PausedGetter(const PropertyCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.paused getter called" << endl;
      // TODO: Return the paused state
      info.GetReturnValue().Set(true);
    }

    void HTMLAudioElement::CurrentTimeGetter(const PropertyCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.currentTime getter called" << endl;
      // TODO: Return the current playback time
      info.GetReturnValue().Set(0.0);
    }

    void HTMLAudioElement::CurrentTimeSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      cout << "HTMLAudioElement.currentTime setter called" << endl;
      // TODO: Set the current playback time
    }

    void HTMLAudioElement::DurationGetter(const PropertyCallbackInfo<Value> &info)
    {
      cout << "HTMLAudioElement.duration getter called" << endl;
      // TODO: Return the media duration
      info.GetReturnValue().Set(Number::New(info.GetIsolate(), std::numeric_limits<double>::quiet_NaN()));
    }
  }
} // namespace endor
