#pragma once

#include <node/v8.h>

namespace script_bindings
{
  namespace dom
  {
    /**
     * Initialize all DOM classes and register them with V8.
     * This function should be called once when setting up the V8 context.
     * 
     * @param isolate The V8 isolate to register classes with
     * @param context The V8 context to attach DOM classes to
     */
    void Initialize(v8::Isolate *isolate, v8::Local<v8::Context> context);
  }
}