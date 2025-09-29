#include "./html_media_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    using namespace v8;

    void HTMLMediaElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

      // Media-specific properties
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "src").ToLocalChecked(), SrcGetter, SrcSetter);
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "currentTime").ToLocalChecked(), CurrentTimeGetter, CurrentTimeSetter);
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "duration").ToLocalChecked(), DurationGetter);
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "paused").ToLocalChecked(), PausedGetter);
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "muted").ToLocalChecked(), MutedGetter, MutedSetter);
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "volume").ToLocalChecked(), VolumeGetter, VolumeSetter);
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "readyState").ToLocalChecked(), ReadyStateGetter);

      // Media control methods
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "play").ToLocalChecked(),
                             FunctionTemplate::New(isolate, Play));
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "pause").ToLocalChecked(),
                             FunctionTemplate::New(isolate, Pause));
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "load").ToLocalChecked(),
                             FunctionTemplate::New(isolate, Load));
    }

    HTMLMediaElement::HTMLMediaElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLMediaElementBase(isolate, args)
    {
    }

    // Property getters
    void HTMLMediaElement::SrcGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        std::string src = wrapper->inner()->getSrc();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, src.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    void HTMLMediaElement::CurrentTimeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        double currentTime = wrapper->inner()->currentTime;
        info.GetReturnValue().Set(Number::New(isolate, currentTime));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0.0));
      }
    }

    void HTMLMediaElement::DurationGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        double duration = wrapper->inner()->duration();
        info.GetReturnValue().Set(Number::New(isolate, duration));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0.0));
      }
    }

    void HTMLMediaElement::PausedGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        bool paused = wrapper->inner()->paused();
        info.GetReturnValue().Set(Boolean::New(isolate, paused));
      }
      else
      {
        info.GetReturnValue().Set(Boolean::New(isolate, true));
      }
    }

    void HTMLMediaElement::MutedGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        bool muted = wrapper->inner()->muted();
        info.GetReturnValue().Set(Boolean::New(isolate, muted));
      }
      else
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
      }
    }

    void HTMLMediaElement::VolumeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        double volume = wrapper->inner()->getVolume();
        info.GetReturnValue().Set(Number::New(isolate, volume));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 1.0));
      }
    }

    void HTMLMediaElement::ReadyStateGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        int readyState = static_cast<int>(wrapper->inner()->readyState);
        info.GetReturnValue().Set(Integer::New(isolate, readyState));
      }
      else
      {
        info.GetReturnValue().Set(Integer::New(isolate, 0));
      }
    }

    // Property setters
    void HTMLMediaElement::SrcSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner() && value->IsString())
      {
        String::Utf8Value src(isolate, value);
        wrapper->inner()->setSrc(*src);
      }
    }

    void HTMLMediaElement::CurrentTimeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner() && value->IsNumber())
      {
        double currentTime = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0);
        wrapper->inner()->currentTime = currentTime;
      }
    }

    void HTMLMediaElement::MutedSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        wrapper->inner()->setMuted(value->BooleanValue(isolate));
      }
    }

    void HTMLMediaElement::VolumeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner() && value->IsNumber())
      {
        double volume = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(1.0);
        // Clamp volume between 0.0 and 1.0
        volume = std::max(0.0, std::min(1.0, volume));
        wrapper->inner()->setVolume(volume);
      }
    }

    // Methods
    void HTMLMediaElement::Play(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        wrapper->inner()->play();
        // Return a Promise (for now, just return undefined)
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid media element").ToLocalChecked()));
      }
    }

    void HTMLMediaElement::Pause(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        wrapper->inner()->pause();
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid media element").ToLocalChecked()));
      }
    }

    void HTMLMediaElement::Load(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->inner())
      {
        wrapper->inner()->load();
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid media element").ToLocalChecked()));
      }
    }
  }
}
