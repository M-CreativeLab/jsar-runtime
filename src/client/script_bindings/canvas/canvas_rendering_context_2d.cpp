#include "./canvas_rendering_context_2d.hpp"
#include "./canvas_rendering_context_2d-inl.hpp"
#include "./image_data.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings::canvas_bindings
  {
    CanvasRenderingContext2D::CanvasRenderingContext2D(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : CanvasRenderingContext2DBase(isolate, args)
    {
    }
  }
} // namespace endor
