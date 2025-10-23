#include "./html_media_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    using namespace std;
    using namespace v8;

    void HTMLMediaElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Media-specific properties
      InstanceAccessor(isolate,
                       prototype,
                       "src",
                       &HTMLMediaElement::SrcGetter,
                       &HTMLMediaElement::SrcSetter);
      InstanceAccessor(isolate,
                       prototype,
                       "currentTime",
                       &HTMLMediaElement::CurrentTimeGetter,
                       &HTMLMediaElement::CurrentTimeSetter);
      InstanceAccessor(isolate, prototype, "muted", &HTMLMediaElement::MutedGetter, &HTMLMediaElement::MutedSetter);
      InstanceAccessor(isolate, prototype, "volume", &HTMLMediaElement::VolumeGetter, &HTMLMediaElement::VolumeSetter);
      InstanceAccessor(isolate, prototype, "duration", &HTMLMediaElement::DurationGetter, nullptr);
      InstanceAccessor(isolate, prototype, "paused", &HTMLMediaElement::PausedGetter, nullptr);
      InstanceAccessor(isolate, prototype, "readyState", &HTMLMediaElement::ReadyStateGetter, nullptr);

      // Media control methods
      InstanceMethod(isolate, prototype, "play", &HTMLMediaElement::Play);
      InstanceMethod(isolate, prototype, "pause", &HTMLMediaElement::Pause);
      InstanceMethod(isolate, prototype, "load", &HTMLMediaElement::Load);
    }

    HTMLMediaElement::HTMLMediaElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLMediaElementBase(isolate, args)
    {
    }

    // Property getters
    void HTMLMediaElement::SrcGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        std::string src = wrapper->handle()->getSrc();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, src.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    void HTMLMediaElement::CurrentTimeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        double currentTime = wrapper->handle()->currentTime;
        info.GetReturnValue().Set(Number::New(isolate, currentTime));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0.0));
      }
    }

    void HTMLMediaElement::DurationGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        double duration = wrapper->handle()->duration();
        info.GetReturnValue().Set(Number::New(isolate, duration));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0.0));
      }
    }

    void HTMLMediaElement::PausedGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        bool paused = wrapper->handle()->paused();
        info.GetReturnValue().Set(Boolean::New(isolate, paused));
      }
      else
      {
        info.GetReturnValue().Set(Boolean::New(isolate, true));
      }
    }

    void HTMLMediaElement::MutedGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        bool muted = wrapper->handle()->muted();
        info.GetReturnValue().Set(Boolean::New(isolate, muted));
      }
      else
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
      }
    }

    void HTMLMediaElement::VolumeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        double volume = wrapper->handle()->getVolume();
        info.GetReturnValue().Set(Number::New(isolate, volume));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 1.0));
      }
    }

    void HTMLMediaElement::ReadyStateGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        int readyState = static_cast<int>(wrapper->handle()->readyState);
        info.GetReturnValue().Set(Integer::New(isolate, readyState));
      }
      else
      {
        info.GetReturnValue().Set(Integer::New(isolate, 0));
      }
    }

    // Property setters
    void HTMLMediaElement::SrcSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle() && value->IsString())
      {
        String::Utf8Value src(isolate, value);
        wrapper->handle()->setSrc(*src);
      }
    }

    void HTMLMediaElement::CurrentTimeSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle() && value->IsNumber())
      {
        double currentTime = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0);
        wrapper->handle()->currentTime = currentTime;
      }
    }

    void HTMLMediaElement::MutedSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        wrapper->handle()->setMuted(value->BooleanValue(isolate));
      }
    }

    void HTMLMediaElement::VolumeSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle() && value->IsNumber())
      {
        double volume = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(1.0);
        // Clamp volume between 0.0 and 1.0
        volume = std::max(0.0, std::min(1.0, volume));
        wrapper->handle()->setVolume(volume);
      }
    }

    // Methods
    void HTMLMediaElement::Play(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        wrapper->handle()->play();
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

      if (wrapper && wrapper->handle())
      {
        wrapper->handle()->pause();
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

      if (wrapper && wrapper->handle())
      {
        wrapper->handle()->load();
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid media element").ToLocalChecked()));
      }
    }
  }
}
