#pragma once

#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * Initialize all WebGL API bindings with V8.
     * 
     * @param context The V8 context to initialize bindings in.
     */
    void Initialize(v8::Local<v8::Context> context);
  }
}