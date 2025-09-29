#include <client/script_bindings/webxr/xr_system.hpp>
#include "navigator.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  Navigator::Navigator(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : NavigatorBase(isolate, args)
  {
    // Navigator constructor - typically not called directly
  }

  void Navigator::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    auto prototype = tpl->PrototypeTemplate();

    // Browser identification properties
    InstanceReadonlyAccessor(isolate, prototype, "userAgent", &Navigator::UserAgentGetter);
    InstanceReadonlyAccessor(isolate, prototype, "platform", &Navigator::PlatformGetter);
    InstanceReadonlyAccessor(isolate, prototype, "vendor", &Navigator::VendorGetter);
    InstanceReadonlyAccessor(isolate, prototype, "vendorSub", &Navigator::VendorSubGetter);
    InstanceReadonlyAccessor(isolate, prototype, "product", &Navigator::ProductGetter);
    InstanceReadonlyAccessor(isolate, prototype, "productSub", &Navigator::ProductSubGetter);

    // Browser capabilities properties
    InstanceReadonlyAccessor(isolate, prototype, "onLine", &Navigator::OnLineGetter);
    InstanceReadonlyAccessor(isolate, prototype, "cookieEnabled", &Navigator::CookieEnabledGetter);
    InstanceReadonlyAccessor(isolate, prototype, "doNotTrack", &Navigator::DoNotTrackGetter);
    InstanceReadonlyAccessor(isolate, prototype, "hardwareConcurrency", &Navigator::HardwareConcurrencyGetter);
    InstanceReadonlyAccessor(isolate, prototype, "maxTouchPoints", &Navigator::MaxTouchPointsGetter);

    // Language support properties
    InstanceReadonlyAccessor(isolate, prototype, "language", &Navigator::LanguageGetter);
    InstanceReadonlyAccessor(isolate, prototype, "languages", &Navigator::LanguagesGetter);

    // User preferences
    InstanceReadonlyAccessor(isolate, prototype, "colorScheme", &Navigator::ColorSchemeGetter);

    // Platform features (for capability detection)
    InstanceReadonlyAccessor(isolate, prototype, "webgl", &Navigator::WebGLGetter);
    InstanceReadonlyAccessor(isolate, prototype, "xr", &Navigator::WebXRGetter);
    InstanceReadonlyAccessor(isolate, prototype, "serviceWorker", &Navigator::ServiceWorkerGetter);
    InstanceReadonlyAccessor(isolate, prototype, "geolocation", &Navigator::GeolocationGetter);
    InstanceReadonlyAccessor(isolate, prototype, "mediaDevices", &Navigator::MediaDevicesGetter);
    InstanceReadonlyAccessor(isolate, prototype, "permissions", &Navigator::PermissionsGetter);
    InstanceReadonlyAccessor(isolate, prototype, "getBattery", &Navigator::BatteryGetter);
    InstanceReadonlyAccessor(isolate, prototype, "clipboard", &Navigator::ClipboardGetter);
    InstanceReadonlyAccessor(isolate, prototype, "storageQuota", &Navigator::StorageQuotaGetter);

    // Methods
    InstanceMethod(isolate, prototype, "javaEnabled", &Navigator::JavaEnabled);
  }

  Local<Object> Navigator::NewInstance(Isolate *isolate, std::shared_ptr<browser::Navigator> nativeNavigator)
  {
    EscapableHandleScope scope(isolate);
    return nativeNavigator != nullptr
             ? scope.Escape(NavigatorBase::NewInstance(isolate, nativeNavigator).As<Object>())
             : scope.Escape(Local<Object>());
  }

  // Browser identification property getters
  void Navigator::UserAgentGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    string userAgent = handle()->userAgent();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  userAgent.c_str())
                                .ToLocalChecked());
  }

  void Navigator::PlatformGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto platform = handle()->platform();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  platform.c_str())
                                .ToLocalChecked());
  }

  void Navigator::VendorGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto vendor = handle()->vendor();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, vendor.c_str()).ToLocalChecked());
  }

  void Navigator::VendorSubGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto vendorSub = handle()->vendorSub();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, vendorSub.c_str()).ToLocalChecked());
  }

  void Navigator::ProductGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto product = handle()->product();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, product.c_str()).ToLocalChecked());
  }

  void Navigator::ProductSubGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto productSub = handle()->productSub();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, productSub.c_str()).ToLocalChecked());
  }

  // Browser capabilities property getters
  void Navigator::OnLineGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->isOnline());
  }

  void Navigator::CookieEnabledGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->isCookieEnabled());
  }

  void Navigator::DoNotTrackGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->isDoNotTrack());
  }

  void Navigator::HardwareConcurrencyGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->hardwareConcurrency());
  }

  void Navigator::MaxTouchPointsGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(static_cast<int32_t>(handle()->maxTouchPoints()));
  }

  // Language support property getters
  void Navigator::LanguageGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto language = handle()->language();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, language.c_str()).ToLocalChecked());
  }

  void Navigator::LanguagesGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto context = isolate->GetCurrentContext();

    auto languages = handle()->languages();
    auto array = Array::New(isolate, static_cast<int>(languages.size()));

    for (size_t i = 0; i < languages.size(); ++i)
    {
      auto languageStr = String::NewFromUtf8(isolate, languages[i].c_str()).ToLocalChecked();
      array->Set(context,
                 static_cast<uint32_t>(i),
                 languageStr)
        .ToChecked();
    }
    info.GetReturnValue().Set(array);
  }

  // User preferences property getters
  void Navigator::JavaEnabledGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->isJavaEnabled());
  }

  void Navigator::ColorSchemeGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto colorScheme = handle()->colorScheme();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, colorScheme.c_str()).ToLocalChecked());
  }

  // Platform features property getters
  void Navigator::WebGLGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->hasWebGL());
  }

  void Navigator::WebXRGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto xrValue = webxr_bindings::XRSystem::GetOrNewInstance(isolate, handle()->getXRSystem());
    info.GetReturnValue().Set(xrValue);
  }

  void Navigator::ServiceWorkerGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO: Return actual ServiceWorkerContainer instance
    info.GetReturnValue().Set(Undefined(isolate));
  }

  void Navigator::GeolocationGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO: Return actual Geolocation instance
    info.GetReturnValue().Set(Undefined(isolate));
  }

  void Navigator::MediaDevicesGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO: Return actual MediaDevices instance
    info.GetReturnValue().Set(Undefined(isolate));
  }

  void Navigator::PermissionsGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO: Return actual Permissions instance
    info.GetReturnValue().Set(Undefined(isolate));
  }

  void Navigator::BatteryGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO: Return actual Battery instance
    info.GetReturnValue().Set(Undefined(isolate));
  }

  void Navigator::ClipboardGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO: Return actual Clipboard instance
    info.GetReturnValue().Set(Undefined(isolate));
  }

  void Navigator::StorageQuotaGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(static_cast<double>(handle()->getStorageQuota()));
  }

  // Methods
  void Navigator::JavaEnabled(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->isJavaEnabled());
  }
}
