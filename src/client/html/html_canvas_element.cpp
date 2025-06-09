#include "./html_canvas_element.hpp"

namespace dom
{
  using namespace std;

  void HTMLCanvasElement::createdCallback(bool from_scripting)
  {
    HTMLElement::createdCallback(from_scripting);
    canvasImpl_ = make_shared<canvas::Canvas>();
  }

  shared_ptr<CanvasRenderingContext> HTMLCanvasElement::getContext(const string &contextTypeStr)
  {
    canvas::RenderingContextType contextType = canvas::RenderingContextType::Unset;
    if (contextTypeStr == "2d")
      contextType = canvas::RenderingContextType::RenderingContext2D;
    else if (contextTypeStr == "webgl")
      contextType = canvas::RenderingContextType::WebGL;
    else if (contextTypeStr == "webgl2")
      contextType = canvas::RenderingContextType::WebGL2;
    else if (contextTypeStr == "bitmaprenderer")
      contextType = canvas::RenderingContextType::BitmapRenderer;
    else
      return nullptr;

    return canvasImpl_->getContext(contextType);
  }
}
