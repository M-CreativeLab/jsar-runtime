#include <client/layout/layout_html_canvas.hpp>
#include "./html_canvas_element.hpp"

namespace dom
{
  using namespace std;

  void HTMLCanvasElement::createdCallback(bool from_scripting)
  {
    HTMLElement::createdCallback(from_scripting);

    auto on_pixels_updated = [this]()
    {
      auto canvasBox = dynamic_pointer_cast<client_layout::LayoutHTMLCanvas>(principalBox());
      assert(canvasBox != nullptr && "The image box is not created yet.");
      canvasBox->markCanvasAsDirty();
    };
    canvas_impl_ = make_shared<canvas::Canvas>();
    canvas_impl_->setPixelsUpdatedCallback(on_pixels_updated);
  }

  void HTMLCanvasElement::connectedCallback()
  {
    HTMLElement::connectedCallback();
    assert(canvas_impl_ != nullptr);

    auto canvasBox = dynamic_pointer_cast<client_layout::LayoutHTMLCanvas>(principalBox());
    assert(canvasBox != nullptr && "The image box is not created yet.");
    canvasBox->setDrawingBitmap(canvas_impl_->getSkBitmap());
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

    return canvas_impl_->getContext(contextType);
  }
}
