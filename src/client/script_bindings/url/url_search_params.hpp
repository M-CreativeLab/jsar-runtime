#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/scripting_base/iterator_protocol_impl.hpp>
#include <client/url/url_search_params.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace url_bindings
    {
      class URLSearchParams;
      class URLSearchParamsIterator;

      using URLSearchParamsBase = scripting_base::ObjectWrap<URLSearchParams, client_url::URLSearchParams>;
      using URLSearchParamsIteratorBase = scripting_base::ObjectWrap<URLSearchParamsIterator, void>;

      class URLSearchParams : public URLSearchParamsBase,
                              public scripting_base::iteration_protocol::Iterable
      {
        using URLSearchParamsBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "URLSearchParams";
        }
        static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);
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
        void GetIterator(const v8::FunctionCallbackInfo<v8::Value> &args) override;
      };

      class URLSearchParamsIterator : public URLSearchParamsIteratorBase,
                                      public scripting_base::iteration_protocol::Iterator
      {
        using URLSearchParamsIteratorBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "URLSearchParams.Iterator";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, URLSearchParams *params);

      private:
        void Next(const v8::FunctionCallbackInfo<v8::Value> &args) override;
        void Return(const v8::FunctionCallbackInfo<v8::Value> &args) override;
        void Throw(const v8::FunctionCallbackInfo<v8::Value> &args) override;

      private:
        int current_index_ = 0;
        v8::Global<v8::Value> params_handle_;
      };
    }
  }
} // namespace endor
