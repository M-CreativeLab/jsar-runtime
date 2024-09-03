#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/parser.hpp"

namespace dombinding
{
  class DOMParser : public Napi::ObjectWrap<DOMParser>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    DOMParser(const Napi::CallbackInfo &info);
    ~DOMParser();

  private:
    Napi::Value ParseFromString(const Napi::CallbackInfo &info);

  private:
    dom::DOMParser parser;

  private:
    static Napi::FunctionReference *constructor;
  };
}
