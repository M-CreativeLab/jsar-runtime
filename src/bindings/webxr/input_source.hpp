#pragma once

#include <napi.h>
#include "common.hpp"

namespace bindings
{
  class XRInputSource : public Napi::ObjectWrap<XRInputSource>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env);
    XRInputSource(const Napi::CallbackInfo &info);
    ~XRInputSource();

  private:
    Napi::Value GamepadGetter(const Napi::CallbackInfo &info);
    Napi::Value GripSpaceGetter(const Napi::CallbackInfo &info);
    Napi::Value HandGetter(const Napi::CallbackInfo &info);
    Napi::Value HandnessGetter(const Napi::CallbackInfo &info);
    Napi::Value TargetRayModeGetter(const Napi::CallbackInfo &info);
    Napi::Value TargetRaySpaceGetter(const Napi::CallbackInfo &info);

  public:
    /** Fields */

  private:
    static Napi::FunctionReference *constructor;
  };

  class XRInputSourceArray : public Napi::ObjectWrap<XRInputSourceArray>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env);
    XRInputSourceArray(const Napi::CallbackInfo &info);
    ~XRInputSourceArray();

  private:
    Napi::Value LengthGetter(const Napi::CallbackInfo &info);
    Napi::Value IndexGetter(const Napi::CallbackInfo &info);
    Napi::Value Entries(const Napi::CallbackInfo &info);
    Napi::Value ForEach(const Napi::CallbackInfo &info);
    Napi::Value Keys(const Napi::CallbackInfo &info);
    Napi::Value Values(const Napi::CallbackInfo &info);

  public:
    Napi::Reference<Napi::Array> inputSources;

  private:
    static Napi::FunctionReference *constructor;
  };
}
