#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/dom/events/error_event.hpp>
#include <client/script_bindings/event.hpp>

namespace endor
{
  namespace script_bindings::event_bindings
  {
    class ErrorEvent;
    using ErrorEventBase = scripting_base::ObjectWrap<ErrorEvent, dom::events::ErrorEvent, Event>;

    class ErrorEvent : public ErrorEventBase
    {
      using ErrorEventBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "ErrorEvent";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      ErrorEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      void MessageGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void FilenameGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void LinenoGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ColnoGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ErrorGetter(const v8::FunctionCallbackInfo<v8::Value> &info);

    private:
      v8::Global<v8::Value> error_handle_;
    };
  }
} // namespace endor
