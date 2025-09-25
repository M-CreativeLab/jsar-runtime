#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/browser/navigator.hpp>

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
    static void UserAgentGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void PlatformGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void VendorGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void VendorSubGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void ProductGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void ProductSubGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // Browser capabilities property getters
    static void OnLineGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void CookieEnabledGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void DoNotTrackGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void HardwareConcurrencyGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void MaxTouchPointsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // Language support property getters
    static void LanguageGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void LanguagesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // User preferences property getters
    static void JavaEnabledGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void ColorSchemeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // Platform features property getters (for capability detection)
    static void WebGLGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void WebXRGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void ServiceWorkerGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void GeolocationGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void MediaDevicesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void PermissionsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void BatteryGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void ClipboardGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void StorageQuotaGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // Methods
    static void JavaEnabled(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}