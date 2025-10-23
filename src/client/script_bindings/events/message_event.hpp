#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/dom/events/message_event.hpp>
#include <client/script_bindings/event.hpp>

namespace script_bindings::event_bindings
{
  class MessageEvent;
  using MessageEventBase = scripting_base::ObjectWrap<MessageEvent,
                                                      dom::events::MessageEvent,
                                                      Event>;

  /**
   * Event wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::Event objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM Event interface including properties like type, bubbles,
   * cancelable and methods like preventDefault().
   */
  class MessageEvent : public MessageEventBase
  {
    using MessageEventBase::ObjectWrap;

  public:
    static std::string Name()
    {
      return "MessageEvent";
    }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

  public:
    MessageEvent(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    void DataGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void OriginGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void SourceGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void PortsGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

  private:
    v8::Global<v8::Value> data_handle_;
  };
}
