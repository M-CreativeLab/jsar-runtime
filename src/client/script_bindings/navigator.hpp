#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/browser/navigator.hpp>

namespace endor
{
  namespace script_bindings
  {
    class Navigator;
    using NavigatorBase = scripting_base::ObjectWrap<Navigator, browser::Navigator>;

    /**
   * Navigator wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps browser::Navigator objects for use in V8 JavaScript execution contexts.
   * It provides the standard Web API Navigator interface for browser capability detection.
   */
    class Navigator : public NavigatorBase
    {
      using NavigatorBase::ObjectWrap;

    public:
      /**
     * The name of the Navigator class for V8.
     */
      static std::string Name()
      {
        return "Navigator";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<browser::Navigator> nativeNavigator);

    public:
      Navigator(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Browser identification property getters
      void UserAgentGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void PlatformGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void VendorGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void VendorSubGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ProductGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ProductSubGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Browser capabilities property getters
      void OnLineGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void CookieEnabledGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void DoNotTrackGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HardwareConcurrencyGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void MaxTouchPointsGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Language support property getters
      void LanguageGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void LanguagesGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // User preferences property getters
      void JavaEnabledGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ColorSchemeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Platform features property getters (for capability detection)
      void WebGLGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void WebXRGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ServiceWorkerGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void GeolocationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void MediaDevicesGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void PermissionsGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void BatteryGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ClipboardGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void StorageQuotaGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      void JavaEnabled(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
} // namespace endor
