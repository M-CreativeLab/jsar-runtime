#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/event_target.hpp>
#include <client/dom/notification.hpp>

namespace script_bindings
{
  class Notification;
  using NotificationBase = scripting_base::ObjectWrap<Notification, ::dom::Notification, EventTarget>;

  /**
   * JavaScript binding for the Notification API.
   * 
   * This class wraps the dom::Notification class for use in V8 JavaScript contexts.
   * It provides the standard Web API Notification interface for displaying system notifications.
   * 
   * @see https://www.w3.org/TR/notifications/
   * @see https://developer.mozilla.org/en-US/docs/Web/API/Notification
   */
  class Notification : public NotificationBase
  {
    using NotificationBase::ObjectWrap;

  public:
    /**
     * The name of the Notification class for V8.
     */
    static std::string Name()
    {
      return "Notification";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Notification> nativeNotification);

  public:
    Notification(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Static property getters
    static void PermissionGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // Static methods
    static void RequestPermission(const v8::FunctionCallbackInfo<v8::Value> &info);

    // Instance property getters
    void TitleGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void DirGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void LangGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void BodyGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void TagGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void IconGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void BadgeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void SoundGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void RenotifyGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void RequireInteractionGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void SilentGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void DataGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

    // Event handler property getters/setters
    void OnShowGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void OnShowSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    void OnClickGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void OnClickSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    void OnCloseGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void OnCloseSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    void OnErrorGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void OnErrorSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

    // Instance methods
    void Close(const v8::FunctionCallbackInfo<v8::Value> &info);

  private:
    // Event handler storage
    v8::Global<v8::Function> on_show_;
    v8::Global<v8::Function> on_click_;
    v8::Global<v8::Function> on_close_;
    v8::Global<v8::Function> on_error_;
  };
}
