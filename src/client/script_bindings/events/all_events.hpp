#pragma once

#include <client/script_bindings/event.hpp>
#include "./ui_event.hpp"
#include "./mouse_event.hpp"
#include "./pointer_event.hpp"

namespace script_bindings
{
  namespace event_bindings
  {
    /**
     * Create a new Event object in V8 from a native dom::Event.
     * 
     * @param isolate The V8 isolate.
     * @param nativeEvent The native dom::Event to wrap.
     * @returns A V8 Object representing the event.
     */
    v8::Local<v8::Object> MakeEvent(v8::Isolate *isolate, dom::Event *nativeEvent);
  }
}
