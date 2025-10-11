#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/graphics/webgl_object.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLObject;
    using WebGLObjectBase = scripting_base::ObjectWrap<WebGLObject, client_graphics::WebGLObject>;

    /**
     * WebGLObject V8 binding for base WebGL object functionality.
     * 
     * This class wraps webgl::WebGLObject objects for use in V8 JavaScript execution contexts.
     * It provides the base class for all WebGL objects.
     */
    class WebGLObject : public WebGLObjectBase
    {
      using WebGLObjectBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLObject class for V8.
       */
      static std::string Name()
      {
        return "WebGLObject";
      }

      /**
       * Configure the V8 function template with WebGLObject properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLObject> nativeObject);

    public:
      WebGLObject(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings