#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_vertex_array.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      class WebGLVertexArray;
      using WebGLVertexArrayBase = scripting_base::ObjectWrap<WebGLVertexArray,
                                                              client_graphics::WebGLVertexArray,
                                                              WebGLObject>;

      class WebGLVertexArray : public WebGLVertexArrayBase
      {
        using WebGLVertexArrayBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "WebGLVertexArray";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        WebGLVertexArray(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };

    } // namespace webgl
  } // namespace script_bindings
} // namespace endor