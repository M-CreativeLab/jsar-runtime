#include "./notification.hpp"
#include <client/scripting_base/v8_utils.hpp>

using namespace std;
using namespace v8;

namespace script_bindings
{
  Notification::Notification(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : NotificationBase(isolate, args)
  {
    HandleScope scope(isolate);

    // Parse constructor arguments: new Notification(title, options)
    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "Notification", "1 argument required, but only 0 present.")));
      return;
    }

    // Get title (required)
    String::Utf8Value titleValue(isolate, args[0]);
    string title = *titleValue ? *titleValue : "";

    // Parse options (optional)
    dom::NotificationOptions options;
    if (args.Length() >= 2 && args[1]->IsObject())
    {
      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> optionsObj = args[1].As<Object>();

      // dir
      Local<String> dirKey = String::NewFromUtf8(isolate, "dir").ToLocalChecked();
      if (optionsObj->Has(context, dirKey).FromMaybe(false))
      {
        Local<Value> dirValue = optionsObj->Get(context, dirKey).ToLocalChecked();
        String::Utf8Value dirStr(isolate, dirValue);
        string dirString = *dirStr ? *dirStr : "auto";
        options.dir = dom::StringToNotificationDirection(dirString);
      }

      // lang
      Local<String> langKey = String::NewFromUtf8(isolate, "lang").ToLocalChecked();
      if (optionsObj->Has(context, langKey).FromMaybe(false))
      {
        Local<Value> langValue = optionsObj->Get(context, langKey).ToLocalChecked();
        String::Utf8Value langStr(isolate, langValue);
        options.lang = *langStr ? *langStr : "";
      }

      // body
      Local<String> bodyKey = String::NewFromUtf8(isolate, "body").ToLocalChecked();
      if (optionsObj->Has(context, bodyKey).FromMaybe(false))
      {
        Local<Value> bodyValue = optionsObj->Get(context, bodyKey).ToLocalChecked();
        String::Utf8Value bodyStr(isolate, bodyValue);
        options.body = *bodyStr ? *bodyStr : "";
      }

      // tag
      Local<String> tagKey = String::NewFromUtf8(isolate, "tag").ToLocalChecked();
      if (optionsObj->Has(context, tagKey).FromMaybe(false))
      {
        Local<Value> tagValue = optionsObj->Get(context, tagKey).ToLocalChecked();
        String::Utf8Value tagStr(isolate, tagValue);
        options.tag = *tagStr ? *tagStr : "";
      }

      // icon
      Local<String> iconKey = String::NewFromUtf8(isolate, "icon").ToLocalChecked();
      if (optionsObj->Has(context, iconKey).FromMaybe(false))
      {
        Local<Value> iconValue = optionsObj->Get(context, iconKey).ToLocalChecked();
        String::Utf8Value iconStr(isolate, iconValue);
        options.icon = *iconStr ? *iconStr : "";
      }

      // badge
      Local<String> badgeKey = String::NewFromUtf8(isolate, "badge").ToLocalChecked();
      if (optionsObj->Has(context, badgeKey).FromMaybe(false))
      {
        Local<Value> badgeValue = optionsObj->Get(context, badgeKey).ToLocalChecked();
        String::Utf8Value badgeStr(isolate, badgeValue);
        options.badge = *badgeStr ? *badgeStr : "";
      }

      // sound
      Local<String> soundKey = String::NewFromUtf8(isolate, "sound").ToLocalChecked();
      if (optionsObj->Has(context, soundKey).FromMaybe(false))
      {
        Local<Value> soundValue = optionsObj->Get(context, soundKey).ToLocalChecked();
        String::Utf8Value soundStr(isolate, soundValue);
        options.sound = *soundStr ? *soundStr : "";
      }

      // renotify
      Local<String> renotifyKey = String::NewFromUtf8(isolate, "renotify").ToLocalChecked();
      if (optionsObj->Has(context, renotifyKey).FromMaybe(false))
      {
        Local<Value> renotifyValue = optionsObj->Get(context, renotifyKey).ToLocalChecked();
        options.renotify = renotifyValue->BooleanValue(isolate);
      }

      // requireInteraction
      Local<String> requireInteractionKey = String::NewFromUtf8(isolate, "requireInteraction").ToLocalChecked();
      if (optionsObj->Has(context, requireInteractionKey).FromMaybe(false))
      {
        Local<Value> requireInteractionValue = optionsObj->Get(context, requireInteractionKey).ToLocalChecked();
        options.requireInteraction = requireInteractionValue->BooleanValue(isolate);
      }

      // silent
      Local<String> silentKey = String::NewFromUtf8(isolate, "silent").ToLocalChecked();
      if (optionsObj->Has(context, silentKey).FromMaybe(false))
      {
        Local<Value> silentValue = optionsObj->Get(context, silentKey).ToLocalChecked();
        options.silent = silentValue->BooleanValue(isolate);
      }

      // data - store the value for now (simplified)
      Local<String> dataKey = String::NewFromUtf8(isolate, "data").ToLocalChecked();
      if (optionsObj->Has(context, dataKey).FromMaybe(false))
      {
        // For now, we don't store complex data
        options.data = nullptr;
      }
    }

    // Create the native Notification object
    auto nativeNotification = make_shared<dom::Notification>(title, options);
    setData(nativeNotification);
  }

  void Notification::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    auto prototype = tpl->PrototypeTemplate();

    // Static properties
    StaticAccessor(isolate, tpl, "permission", &Notification::PermissionGetter, nullptr);

    // Static methods
    StaticMethod(isolate, tpl, "requestPermission", &Notification::RequestPermission);

    // Instance properties (all read-only)
    InstanceReadonlyAccessor(isolate, prototype, "title", &Notification::TitleGetter);
    InstanceReadonlyAccessor(isolate, prototype, "dir", &Notification::DirGetter);
    InstanceReadonlyAccessor(isolate, prototype, "lang", &Notification::LangGetter);
    InstanceReadonlyAccessor(isolate, prototype, "body", &Notification::BodyGetter);
    InstanceReadonlyAccessor(isolate, prototype, "tag", &Notification::TagGetter);
    InstanceReadonlyAccessor(isolate, prototype, "icon", &Notification::IconGetter);
    InstanceReadonlyAccessor(isolate, prototype, "badge", &Notification::BadgeGetter);
    InstanceReadonlyAccessor(isolate, prototype, "sound", &Notification::SoundGetter);
    InstanceReadonlyAccessor(isolate, prototype, "renotify", &Notification::RenotifyGetter);
    InstanceReadonlyAccessor(isolate, prototype, "requireInteraction", &Notification::RequireInteractionGetter);
    InstanceReadonlyAccessor(isolate, prototype, "silent", &Notification::SilentGetter);
    InstanceReadonlyAccessor(isolate, prototype, "data", &Notification::DataGetter);

    // Event handlers
    InstanceAccessor(isolate, prototype, "onshow", &Notification::OnShowGetter, &Notification::OnShowSetter);
    InstanceAccessor(isolate, prototype, "onclick", &Notification::OnClickGetter, &Notification::OnClickSetter);
    InstanceAccessor(isolate, prototype, "onclose", &Notification::OnCloseGetter, &Notification::OnCloseSetter);
    InstanceAccessor(isolate, prototype, "onerror", &Notification::OnErrorGetter, &Notification::OnErrorSetter);

    // Instance methods
    InstanceMethod(isolate, prototype, "close", &Notification::Close);
  }

  Local<Object> Notification::NewInstance(Isolate *isolate, shared_ptr<::dom::Notification> nativeNotification)
  {
    EscapableHandleScope scope(isolate);
    return nativeNotification != nullptr
             ? scope.Escape(NotificationBase::NewInstance(isolate, nativeNotification).As<Object>())
             : scope.Escape(Local<Object>());
  }

  // Static property getters
  void Notification::PermissionGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto permission = dom::Notification::permission();
    auto permissionStr = dom::NotificationPermissionToString(permission);
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, permissionStr.c_str()).ToLocalChecked());
  }

  // Static methods
  void Notification::RequestPermission(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // Request permission (stub implementation)
    auto permission = dom::Notification::requestPermission();
    auto permissionStr = dom::NotificationPermissionToString(permission);

    // In a real implementation, this would return a Promise
    // For now, return the permission string directly
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, permissionStr.c_str()).ToLocalChecked());
  }

  // Instance property getters
  void Notification::TitleGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto title = handle()->title();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, title.c_str()).ToLocalChecked());
  }

  void Notification::DirGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto dir = dom::NotificationDirectionToString(handle()->dir());
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, dir.c_str()).ToLocalChecked());
  }

  void Notification::LangGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto lang = handle()->lang();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, lang.c_str()).ToLocalChecked());
  }

  void Notification::BodyGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto body = handle()->body();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, body.c_str()).ToLocalChecked());
  }

  void Notification::TagGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto tag = handle()->tag();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, tag.c_str()).ToLocalChecked());
  }

  void Notification::IconGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto icon = handle()->icon();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, icon.c_str()).ToLocalChecked());
  }

  void Notification::BadgeGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto badge = handle()->badge();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, badge.c_str()).ToLocalChecked());
  }

  void Notification::SoundGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto sound = handle()->sound();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, sound.c_str()).ToLocalChecked());
  }

  void Notification::RenotifyGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->renotify());
  }

  void Notification::RequireInteractionGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->requireInteraction());
  }

  void Notification::SilentGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(handle()->silent());
  }

  void Notification::DataGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // For now, return null for data
    // In a full implementation, we would store and return the actual data
    info.GetReturnValue().SetNull();
  }

  // Event handler property getters/setters
  void Notification::OnShowGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!on_show_.IsEmpty())
    {
      info.GetReturnValue().Set(on_show_.Get(isolate));
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Notification::OnShowSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (value->IsFunction())
    {
      on_show_.Reset(isolate, value.As<Function>());
    }
    else
    {
      on_show_.Reset();
    }
  }

  void Notification::OnClickGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!on_click_.IsEmpty())
    {
      info.GetReturnValue().Set(on_click_.Get(isolate));
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Notification::OnClickSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (value->IsFunction())
    {
      on_click_.Reset(isolate, value.As<Function>());
    }
    else
    {
      on_click_.Reset();
    }
  }

  void Notification::OnCloseGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!on_close_.IsEmpty())
    {
      info.GetReturnValue().Set(on_close_.Get(isolate));
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Notification::OnCloseSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (value->IsFunction())
    {
      on_close_.Reset(isolate, value.As<Function>());
    }
    else
    {
      on_close_.Reset();
    }
  }

  void Notification::OnErrorGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!on_error_.IsEmpty())
    {
      info.GetReturnValue().Set(on_error_.Get(isolate));
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Notification::OnErrorSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (value->IsFunction())
    {
      on_error_.Reset(isolate, value.As<Function>());
    }
    else
    {
      on_error_.Reset();
    }
  }

  // Instance methods
  void Notification::Close(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    handle()->close();
    info.GetReturnValue().SetUndefined();
  }
}
