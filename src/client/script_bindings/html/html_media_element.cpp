#include "./html_media_element.hpp"
#include <client/dom/html_media_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    using namespace v8;

    void HTMLMediaElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Media-specific properties
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "src").ToLocalChecked(), SrcGetter, SrcSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "currentTime").ToLocalChecked(), CurrentTimeGetter, CurrentTimeSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "duration").ToLocalChecked(), DurationGetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "paused").ToLocalChecked(), PausedGetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "muted").ToLocalChecked(), MutedGetter, MutedSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "volume").ToLocalChecked(), VolumeGetter, VolumeSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "readyState").ToLocalChecked(), ReadyStateGetter);

      // Media control methods
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "play").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Play));
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "pause").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Pause));
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "load").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Load));
    }

    Local<Object> HTMLMediaElement::NewInstance(Isolate *isolate, std::shared_ptr<dom::HTMLMediaElement> nativeMediaElement)
    {
      EscapableHandleScope scope(isolate);
      if (nativeMediaElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(HTMLMediaElementBase::NewInstance(isolate, nativeMediaElement).As<Object>());
      }
    }

    Local<Function> HTMLMediaElement::Initialize(Isolate *isolate)
    {
      return HTMLMediaElementBase::Initialize(isolate);
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        std::string src = wrapper->GetNativeInstance()->getSrc();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        double currentTime = wrapper->GetNativeInstance()->getCurrentTime();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        double duration = wrapper->GetNativeInstance()->getDuration();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        bool paused = wrapper->GetNativeInstance()->isPaused();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        bool muted = wrapper->GetNativeInstance()->isMuted();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        double volume = wrapper->GetNativeInstance()->getVolume();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        int readyState = wrapper->GetNativeInstance()->getReadyState();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsString())
      {
        String::Utf8Value src(isolate, value);
        wrapper->GetNativeInstance()->setSrc(*src);
      }
    }

    void HTMLMediaElement::CurrentTimeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsNumber())
      {
        double currentTime = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0);
        wrapper->GetNativeInstance()->setCurrentTime(currentTime);
      }
    }

    void HTMLMediaElement::MutedSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        bool muted = value->BooleanValue(isolate);
        wrapper->GetNativeInstance()->setMuted(muted);
      }
    }

    void HTMLMediaElement::VolumeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsNumber())
      {
        double volume = value->NumberValue(isolate->GetCurrentContext()).FromMaybe(1.0);
        // Clamp volume between 0.0 and 1.0
        volume = std::max(0.0, std::min(1.0, volume));
        wrapper->GetNativeInstance()->setVolume(volume);
      }
    }

    // Methods
    void HTMLMediaElement::Play(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        wrapper->GetNativeInstance()->play();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        wrapper->GetNativeInstance()->pause();
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
      HTMLMediaElement *wrapper = ObjectWrap::Unwrap<HTMLMediaElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        wrapper->GetNativeInstance()->load();
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid media element").ToLocalChecked()));
      }
    }
  }
}