#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/url/url.hpp>

namespace script_bindings
{
  namespace url_bindings
  {
    class URL;
    using URLBase = scripting_base::ObjectWrap<URL, client_url::URL>;

    class URL : public URLBase
    {
      using URLBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "URL";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      URL(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      void HashGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void HashSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void HostGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void HostSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void HostnameGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void HostnameSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void HrefGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void HrefSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void OriginGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void PathnameGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void PathnameSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void PortGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void PortSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void ProtocolGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void ProtocolSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void SearchGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void SearchSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void SearchParamsGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void UsernameGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void UsernameSetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void PasswordGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void PasswordSetter(const v8::FunctionCallbackInfo<v8::Value> &args);

      // Methods
      void ToString(const v8::FunctionCallbackInfo<v8::Value> &args);
      void ToJSON(const v8::FunctionCallbackInfo<v8::Value> &args);

      // Static Methods
      static void CanParse(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void Parse(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void CreateObjectURL(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void RevokeObjectURL(const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
