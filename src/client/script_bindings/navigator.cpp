#include "navigator.hpp"

namespace script_bindings
{
  Navigator::Navigator(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
    : NavigatorBase(isolate, args)
  {
    // Navigator constructor - typically not called directly
  }

  void Navigator::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    v8::HandleScope handle_scope(isolate);
    auto context = isolate->GetCurrentContext();
    auto prototype_template = tpl->PrototypeTemplate();
    auto instance_template = tpl->InstanceTemplate();

    // Browser identification properties
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "userAgent").ToLocalChecked(), UserAgentGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "platform").ToLocalChecked(), PlatformGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "vendor").ToLocalChecked(), VendorGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "vendorSub").ToLocalChecked(), VendorSubGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "product").ToLocalChecked(), ProductGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "productSub").ToLocalChecked(), ProductSubGetter);

    // Browser capabilities properties
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "onLine").ToLocalChecked(), OnLineGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "cookieEnabled").ToLocalChecked(), CookieEnabledGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "doNotTrack").ToLocalChecked(), DoNotTrackGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "hardwareConcurrency").ToLocalChecked(), HardwareConcurrencyGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "maxTouchPoints").ToLocalChecked(), MaxTouchPointsGetter);

    // Language support properties
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "language").ToLocalChecked(), LanguageGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "languages").ToLocalChecked(), LanguagesGetter);

    // User preferences
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "colorScheme").ToLocalChecked(), ColorSchemeGetter);

    // Platform features (for capability detection)
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "webgl").ToLocalChecked(), WebGLGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "xr").ToLocalChecked(), WebXRGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "serviceWorker").ToLocalChecked(), ServiceWorkerGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "geolocation").ToLocalChecked(), GeolocationGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "mediaDevices").ToLocalChecked(), MediaDevicesGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "permissions").ToLocalChecked(), PermissionsGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "getBattery").ToLocalChecked(), BatteryGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "clipboard").ToLocalChecked(), ClipboardGetter);
    instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "storageQuota").ToLocalChecked(), StorageQuotaGetter);

    // Methods
    prototype_template->Set(v8::String::NewFromUtf8(isolate, "javaEnabled").ToLocalChecked(),
                           v8::FunctionTemplate::New(isolate, JavaEnabled));
  }

  v8::Local<v8::Object> Navigator::NewInstance(v8::Isolate *isolate, std::shared_ptr<browser::Navigator> nativeNavigator)
  {
    v8::EscapableHandleScope handle_scope(isolate);
    auto context = isolate->GetCurrentContext();
    
    auto constructor = Navigator::GetConstructorFunction(isolate);
    v8::Local<v8::Object> instance;
    
    if (constructor->NewInstance(context).ToLocal(&instance))
    {
      Navigator::Wrap(isolate, instance, new Navigator(isolate, v8::FunctionCallbackInfo<v8::Value>(nullptr, 0, nullptr)));
      // TODO: Set native navigator instance
    }
    
    return handle_scope.Escape(instance);
  }

  v8::Local<v8::Function> Navigator::Initialize(v8::Isolate *isolate)
  {
    return NavigatorBase::Initialize(isolate);
  }

  // Browser identification property getters
  void Navigator::UserAgentGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto userAgent = navigator->GetUserAgent();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, userAgent.c_str()).ToLocalChecked());
    }
  }

  void Navigator::PlatformGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto platform = navigator->GetPlatform();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, platform.c_str()).ToLocalChecked());
    }
  }

  void Navigator::VendorGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto vendor = navigator->GetVendor();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, vendor.c_str()).ToLocalChecked());
    }
  }

  void Navigator::VendorSubGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto vendorSub = navigator->GetVendorSub();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, vendorSub.c_str()).ToLocalChecked());
    }
  }

  void Navigator::ProductGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto product = navigator->GetProduct();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, product.c_str()).ToLocalChecked());
    }
  }

  void Navigator::ProductSubGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto productSub = navigator->GetProductSub();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, productSub.c_str()).ToLocalChecked());
    }
  }

  // Browser capabilities property getters
  void Navigator::OnLineGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->IsOnLine());
    }
  }

  void Navigator::CookieEnabledGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->IsCookieEnabled());
    }
  }

  void Navigator::DoNotTrackGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->IsDoNotTrack());
    }
  }

  void Navigator::HardwareConcurrencyGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->GetHardwareConcurrency());
    }
  }

  void Navigator::MaxTouchPointsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(static_cast<int32_t>(navigator->GetMaxTouchPoints()));
    }
  }

  // Language support property getters
  void Navigator::LanguageGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto language = navigator->GetLanguage();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, language.c_str()).ToLocalChecked());
    }
  }

  void Navigator::LanguagesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto languages = navigator->GetLanguages();
      auto array = v8::Array::New(isolate, static_cast<int>(languages.size()));
      
      for (size_t i = 0; i < languages.size(); ++i)
      {
        array->Set(context, static_cast<uint32_t>(i), 
                  v8::String::NewFromUtf8(isolate, languages[i].c_str()).ToLocalChecked());
      }
      
      info.GetReturnValue().Set(array);
    }
  }

  // User preferences property getters
  void Navigator::JavaEnabledGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->IsJavaEnabled());
    }
  }

  void Navigator::ColorSchemeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      auto colorScheme = navigator->GetColorScheme();
      info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, colorScheme.c_str()).ToLocalChecked());
    }
  }

  // Platform features property getters
  void Navigator::WebGLGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->HasWebGL());
    }
  }

  void Navigator::WebXRGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      // TODO: Return actual XRSystem instance
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  void Navigator::ServiceWorkerGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      // TODO: Return actual ServiceWorkerContainer instance
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  void Navigator::GeolocationGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      // TODO: Return actual Geolocation instance
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  void Navigator::MediaDevicesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      // TODO: Return actual MediaDevices instance
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  void Navigator::PermissionsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      // TODO: Return actual Permissions instance
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  void Navigator::BatteryGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      // TODO: Return actual Battery instance
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  void Navigator::ClipboardGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      // TODO: Return actual Clipboard instance
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  void Navigator::StorageQuotaGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(static_cast<double>(navigator->GetStorageQuota()));
    }
  }

  // Methods
  void Navigator::JavaEnabled(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    auto navigator = GetNativeInstance(info.Holder());
    if (navigator)
    {
      info.GetReturnValue().Set(navigator->IsJavaEnabled());
    }
  }
}