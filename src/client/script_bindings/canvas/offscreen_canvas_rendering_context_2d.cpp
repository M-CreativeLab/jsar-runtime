#include "./offscreen_canvas_rendering_context_2d.hpp"
#include "./canvas_rendering_context_2d.hpp"

namespace script_bindings::canvas_bindings
{
  using namespace std;
  using namespace v8;

  void OffscreenCanvasRenderingContext2D::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    CanvasRenderingContext2D::ConfigureFunctionTemplate(isolate, tpl);
  }
}
