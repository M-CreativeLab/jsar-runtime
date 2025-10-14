#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/canvas.hpp>
#include <client/canvas/rendering_context2d.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    class OffscreenCanvasRenderingContext2D;
    using OffscreenCanvasRenderingContext2DBase = scripting_base::ObjectWrap<
      OffscreenCanvasRenderingContext2D,
      ::canvas::CanvasRenderingContext2D<::canvas::OffscreenCanvas>>;

    class OffscreenCanvasRenderingContext2D : public OffscreenCanvasRenderingContext2DBase
    {
      using OffscreenCanvasRenderingContext2DBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "OffscreenCanvasRenderingContext2D";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    };
  }
}
