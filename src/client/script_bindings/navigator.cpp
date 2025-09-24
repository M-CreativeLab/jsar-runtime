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
    HandleScope handle_scope(isolate);
    auto context = isolate->GetCurrentContext();
    auto prototype_template = tpl->PrototypeTemplate();
    auto instance_template = tpl->InstanceTemplate();

    // Browser identification properties
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "userAgent").ToLocalChecked(), UserAgentGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "platform").ToLocalChecked(), PlatformGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "vendor").ToLocalChecked(), VendorGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "vendorSub").ToLocalChecked(), VendorSubGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "product").ToLocalChecked(), ProductGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "productSub").ToLocalChecked(), ProductSubGetter);

    // Browser capabilities properties
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "onLine").ToLocalChecked(), OnLineGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "cookieEnabled").ToLocalChecked(), CookieEnabledGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "doNotTrack").ToLocalChecked(), DoNotTrackGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "hardwareConcurrency").ToLocalChecked(), HardwareConcurrencyGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "maxTouchPoints").ToLocalChecked(), MaxTouchPointsGetter);

    // Language support properties
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "language").ToLocalChecked(), LanguageGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "languages").ToLocalChecked(), LanguagesGetter);

    // User preferences
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "colorScheme").ToLocalChecked(), ColorSchemeGetter);

    // Platform features (for capability detection)
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "webgl").ToLocalChecked(), WebGLGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "xr").ToLocalChecked(), WebXRGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "serviceWorker").ToLocalChecked(), ServiceWorkerGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "geolocation").ToLocalChecked(), GeolocationGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "mediaDevices").ToLocalChecked(), MediaDevicesGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "permissions").ToLocalChecked(), PermissionsGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "getBattery").ToLocalChecked(), BatteryGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "clipboard").ToLocalChecked(), ClipboardGetter);
    instance_template->SetAccessor(String::NewFromUtf8(isolate, "storageQuota").ToLocalChecked(), StorageQuotaGetter);

    // Methods
    prototype_template->Set(String::NewFromUtf8(isolate, "javaEnabled").ToLocalChecked(),
                            FunctionTemplate::New(isolate, JavaEnabled));
  }

  Local<Object> Navigator::NewInstance(Isolate *isolate, std::shared_ptr<browser::Navigator> nativeNavigator)
  {
    EscapableHandleScope scope(isolate);
    return nativeNavigator != nullptr
             ? scope.Escape(NavigatorBase::NewInstance(isolate, nativeNavigator).As<Object>())
             : scope.Escape(Local<Object>());
  }

  Local<Function> Navigator::Initialize(Isolate *isolate)
  {
    return NavigatorBase::Initialize(isolate);
  }

  // Browser identification property getters
  void Navigator::UserAgentGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto userAgent = navigator->inner()->GetUserAgent();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, userAgent.c_str()).ToLocalChecked());
    }
  }

  void Navigator::PlatformGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto platform = navigator->inner()->GetPlatform();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, platform.c_str()).ToLocalChecked());
    }
  }

  void Navigator::VendorGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto vendor = navigator->inner()->GetVendor();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, vendor.c_str()).ToLocalChecked());
    }
  }

  void Navigator::VendorSubGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto vendorSub = navigator->inner()->GetVendorSub();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, vendorSub.c_str()).ToLocalChecked());
    }
  }

  void Navigator::ProductGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto product = navigator->inner()->GetProduct();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, product.c_str()).ToLocalChecked());
    }
  }

  void Navigator::ProductSubGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto productSub = navigator->inner()->GetProductSub();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, productSub.c_str()).ToLocalChecked());
    }
  }

  // Browser capabilities property getters
  void Navigator::OnLineGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->inner()->IsOnLine());
    }
  }

  void Navigator::CookieEnabledGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->inner()->IsCookieEnabled());
    }
  }

  void Navigator::DoNotTrackGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->inner()->IsDoNotTrack());
    }
  }

  void Navigator::HardwareConcurrencyGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->inner()->GetHardwareConcurrency());
    }
  }

  void Navigator::MaxTouchPointsGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(static_cast<int32_t>(navigator->inner()->GetMaxTouchPoints()));
    }
  }

  // Language support property getters
  void Navigator::LanguageGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto language = navigator->inner()->GetLanguage();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, language.c_str()).ToLocalChecked());
    }
  }

  void Navigator::LanguagesGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto languages = navigator->inner()->GetLanguages();
      auto array = Array::New(isolate, static_cast<int>(languages.size()));

      for (size_t i = 0; i < languages.size(); ++i)
      {
        array->Set(context, static_cast<uint32_t>(i), String::NewFromUtf8(isolate, languages[i].c_str()).ToLocalChecked());
      }

      info.GetReturnValue().Set(array);
    }
  }

  // User preferences property getters
  void Navigator::JavaEnabledGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->inner()->IsJavaEnabled());
    }
  }

  void Navigator::ColorSchemeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      auto colorScheme = navigator->inner()->GetColorScheme();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, colorScheme.c_str()).ToLocalChecked());
    }
  }

  // Platform features property getters
  void Navigator::WebGLGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->inner()->HasWebGL());
    }
  }

  void Navigator::WebXRGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      // TODO: Return actual XRSystem instance
      info.GetReturnValue().Set(Undefined(info.GetIsolate()));
    }
  }

  void Navigator::ServiceWorkerGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      // TODO: Return actual ServiceWorkerContainer instance
      info.GetReturnValue().Set(Undefined(info.GetIsolate()));
    }
  }

  void Navigator::GeolocationGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      // TODO: Return actual Geolocation instance
      info.GetReturnValue().Set(Undefined(info.GetIsolate()));
    }
  }

  void Navigator::MediaDevicesGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      // TODO: Return actual MediaDevices instance
      info.GetReturnValue().Set(Undefined(info.GetIsolate()));
    }
  }

  void Navigator::PermissionsGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      // TODO: Return actual Permissions instance
      info.GetReturnValue().Set(Undefined(info.GetIsolate()));
    }
  }

  void Navigator::BatteryGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      // TODO: Return actual Battery instance
      info.GetReturnValue().Set(Undefined(info.GetIsolate()));
    }
  }

  void Navigator::ClipboardGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      // TODO: Return actual Clipboard instance
      info.GetReturnValue().Set(Undefined(info.GetIsolate()));
    }
  }

  void Navigator::StorageQuotaGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(static_cast<double>(navigator->inner()->GetStorageQuota()));
    }
  }

  // Methods
  void Navigator::JavaEnabled(const FunctionCallbackInfo<Value> &info)
  {
    auto navigator = Unwrap(info.This());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->inner()->IsJavaEnabled());
    }
  }
}