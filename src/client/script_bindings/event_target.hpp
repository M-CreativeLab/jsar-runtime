#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>

namespace script_bindings
{
  class EventTarget;
  using EventTargetBase = scripting_base::ObjectWrap<EventTarget, ::dom::DOMEventTarget>;

  class EventTarget : public EventTargetBase
  {
    using EventTargetBase::ObjectWrap;

  public:
    static std::string Name()
    {
      return "EventTarget";
    }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

  public:
    virtual dom::DOMEventTargetType eventTargetType() const
    {
      return dom::DOMEventTargetType::kEventTarget;
    }

  private:
    // Event methods
    void AddEventListener(const v8::FunctionCallbackInfo<v8::Value> &info);
    void RemoveEventListener(const v8::FunctionCallbackInfo<v8::Value> &info);
    void DispatchEvent(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}
