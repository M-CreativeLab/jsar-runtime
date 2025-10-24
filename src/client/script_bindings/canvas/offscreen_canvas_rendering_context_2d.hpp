#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/canvas/canvas_rendering_context_2d-inl.hpp>
#include <client/canvas/canvas.hpp>
#include <client/canvas/rendering_context2d.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace canvas_bindings
    {
      class OffscreenCanvasRenderingContext2D : public CanvasRenderingContext2DBase<OffscreenCanvasRenderingContext2D,
                                                                                    canvas::OffscreenCanvas>
      {
        using CanvasRenderingContext2DBase::CanvasRenderingContext2DBase;

      public:
        static std::string Name()
        {
          return "OffscreenCanvasRenderingContext2D";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      };
    }
  }
} // namespace endor
