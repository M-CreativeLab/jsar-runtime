#pragma once

#include <memory>
#include <napi.h>
#include <common/utility.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>

namespace cssombinding
{
  class CSSStyleDeclaration : public scripting_base::ObjectWrap<CSSStyleDeclaration,
                                                                client_cssom::CSSStyleDeclaration>
  {
    using Base = scripting_base::ObjectWrap<CSSStyleDeclaration, client_cssom::CSSStyleDeclaration>;
    using Base::ObjectWrap;

  public:
    static std::string Name()
    {
      return "CSSStyleDeclaration";
    }

    static void Init(Napi::Env);
    static Napi::Value NewInstance(Napi::Env, std::shared_ptr<client_cssom::CSSStyleDeclaration>);
    static inline CSSStyleDeclaration *Unwrap(v8::Local<v8::Object> object)
    {
      return scripting_base::ObjectWrap<CSSStyleDeclaration>::Unwrap(object);
    }
    static void ConfigureFunctionTemplate(v8::Isolate *, v8::Local<v8::FunctionTemplate>);

  private:
    static void GetPropertyPriority(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void GetPropertyValue(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void Item(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void RemoveProperty(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void SetProperty(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ToString(const v8::FunctionCallbackInfo<v8::Value> &args);

    static void PropertyGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void PropertySetter(
      v8::Local<v8::Name> property,
      v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<v8::Value> &info);
    static void PropertyDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean> &info);
    static void PropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info);

  private:
    napi_env napi_env_;
  };
}
