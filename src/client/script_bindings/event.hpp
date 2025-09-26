#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event.hpp>
#include <client/dom/dom_event_target.hpp>

namespace script_bindings
{
  class Event;
  using EventBase = scripting_base::ObjectWrap<Event, ::dom::Event>;

  /**
   * Event wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::Event objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM Event interface including properties like type, bubbles,
   * cancelable and methods like preventDefault().
   */
  class Event : public EventBase
  {
  public:
    /**
     * The name of the Event class for V8.
     */
    static std::string Name()
    {
      return "Event";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Event> nativeEvent);

  public:
    Event(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Property getters
    static void TypeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void BubblesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void CancelableGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void ComposedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

    // Methods
    static void PreventDefault(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void StopPropagation(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}