#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/browser/location.hpp>

namespace endor
{
  namespace script_bindings
  {
    class Location;
    using LocationBase = scripting_base::ObjectWrap<Location, browser::Location>;

    /**
   * Location wrapper for V8 objects using scripting_base::ObjectWrap.
   */
    class Location : public LocationBase
    {
      using LocationBase::ObjectWrap;

    public:
      /**
     * The name of the Navigator class for V8.
     */
      static std::string Name()
      {
        return "Location";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<browser::Location> nativeLocation);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, const std::string &url);

    private:
      // Browser identification property getters
      void HrefGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HrefSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void ProtocolGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ProtocolSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void HostGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HostSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void HostnameGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HostnameSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void PortGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void PortSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void PathnameGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void PathnameSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void SearchGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void SearchSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void HashGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HashSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void OriginGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void OriginSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      void Assign(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Reload(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Replace(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ToString(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
} // namespace endor
