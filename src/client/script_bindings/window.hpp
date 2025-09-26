#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>

namespace script_bindings
{
  enum ContextEmbedderIndex : int
  {
    kSandboxObject = 50,
    kInternalObject,
    kScriptingContextExternal,
    kContextTag,
  };

  class Window;
  using WindowBase = scripting_base::ObjectWrap<Window>;

  class Window : public WindowBase
  {
    using WindowBase::ObjectWrap;

  public:
    static std::string Name()
    {
      return "Window";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static v8::Local<v8::ObjectTemplate> GetInstanceTemplate(v8::Isolate *isolate);
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate);
  };
}
