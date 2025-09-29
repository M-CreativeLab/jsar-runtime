#include "./html_audio_element.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::html_bindings
{
  void HTMLAudioElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Media control methods
    prototypeTemplate->Set(isolate, "play", FunctionTemplate::New(isolate, Play));
    prototypeTemplate->Set(isolate, "pause", FunctionTemplate::New(isolate, Pause));
    prototypeTemplate->Set(isolate, "load", FunctionTemplate::New(isolate, Load));

    // Media properties
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "src").ToLocalChecked(),
                                  SrcGetter,
                                  SrcSetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "volume").ToLocalChecked(),
                                  VolumeGetter,
                                  VolumeSetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "muted").ToLocalChecked(),
                                  MutedGetter,
                                  MutedSetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "paused").ToLocalChecked(),
                                  PausedGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "currentTime").ToLocalChecked(),
                                  CurrentTimeGetter,
                                  CurrentTimeSetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "duration").ToLocalChecked(),
                                  DurationGetter);
  }

  void HTMLAudioElement::AudioConstructor(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    cout << "new Audio() constructor called" << endl;

    // TODO: Create new HTMLAudioElement instance
    // Optional argument: src (string)

    Local<Function> constructor = HTMLAudioElement::GetConstructorFunction(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Value> argv[info.Length()];
    for (int i = 0; i < info.Length(); i++)
      argv[i] = info[i];
    Local<Object> instance = constructor->NewInstance(context, info.Length(), argv).ToLocalChecked();
    info.GetReturnValue().Set(instance);
  }

  HTMLAudioElement::HTMLAudioElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLAudioElementBase(isolate, args)
  {
    // HTMLAudioElement constructor
    cout << "HTMLAudioElement V8 wrapper created" << endl;
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
  void HTMLAudioElement::SrcGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLAudioElement.src getter called" << endl;
    // TODO: Return the current source URL
    info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "").ToLocalChecked());
  }

  void HTMLAudioElement::SrcSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    cout << "HTMLAudioElement.src setter called" << endl;
    // TODO: Set the source URL and trigger loading
  }

  void HTMLAudioElement::VolumeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLAudioElement.volume getter called" << endl;
    // TODO: Return the current volume (0.0 to 1.0)
    info.GetReturnValue().Set(1.0);
  }

  void HTMLAudioElement::VolumeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    cout << "HTMLAudioElement.volume setter called" << endl;
    // TODO: Set the volume level
  }

  void HTMLAudioElement::MutedGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLAudioElement.muted getter called" << endl;
    // TODO: Return the muted state
    info.GetReturnValue().Set(false);
  }

  void HTMLAudioElement::MutedSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    cout << "HTMLAudioElement.muted setter called" << endl;
    // TODO: Set the muted state
  }

  void HTMLAudioElement::PausedGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLAudioElement.paused getter called" << endl;
    // TODO: Return the paused state
    info.GetReturnValue().Set(true);
  }

  void HTMLAudioElement::CurrentTimeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLAudioElement.currentTime getter called" << endl;
    // TODO: Return the current playback time
    info.GetReturnValue().Set(0.0);
  }

  void HTMLAudioElement::CurrentTimeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    cout << "HTMLAudioElement.currentTime setter called" << endl;
    // TODO: Set the current playback time
  }

  void HTMLAudioElement::DurationGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLAudioElement.duration getter called" << endl;
    // TODO: Return the media duration
    info.GetReturnValue().Set(Number::New(info.GetIsolate(), std::numeric_limits<double>::quiet_NaN()));
  }
}
