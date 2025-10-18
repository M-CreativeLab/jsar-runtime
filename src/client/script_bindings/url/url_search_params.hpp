#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/url/url_search_params.hpp>

namespace script_bindings
{
  namespace url_bindings
  {
    class URLSearchParams;
    using URLSearchParamsBase = scripting_base::ObjectWrap<URLSearchParams, client_url::URLSearchParams>;

    class URLSearchParams : public URLSearchParamsBase
    {
      using URLSearchParamsBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "URLSearchParams";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      URLSearchParams(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Properties
      void SizeGetter(const v8::FunctionCallbackInfo<v8::Value> &args);

      // Methods
      void Append(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Delete(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Get(const v8::FunctionCallbackInfo<v8::Value> &args);
      void GetAll(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Has(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Set(const v8::FunctionCallbackInfo<v8::Value> &args);
      void ToString(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Keys(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Values(const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
