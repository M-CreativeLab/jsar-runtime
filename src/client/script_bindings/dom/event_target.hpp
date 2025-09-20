#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>

namespace script_bindings
{
  namespace dom
  {
    /**
     * EventTarget wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::EventTarget objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM EventTarget interface including addEventListener,
     * removeEventListener, and dispatchEvent methods.
     */
    class EventTarget : public scripting_base::ObjectWrap<EventTarget, ::dom::EventTarget>
    {
    public:
      /**
       * The name of the EventTarget class for V8.
       */
      static std::string Name()
      {
        return "EventTarget";
      }

      /**
       * Configure the V8 function template with EventTarget properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Initialize the EventTarget class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      EventTarget(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Event methods
      static void AddEventListener(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void RemoveEventListener(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void DispatchEvent(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}