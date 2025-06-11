#include <assert.h>
#include <client/builtin_scene/scene.hpp>
#include <client/dom/node.hpp>
#include <client/html/html_canvas_element.hpp>

#include "./layout_html_canvas.hpp"

namespace client_layout
{
  using namespace std;
  using namespace dom;
  using namespace builtin_scene;
  using namespace crates::layout2::styles;

  bool LayoutHTMLCanvas::adjustDrawingSize()
  {
    const auto &canvas_element = dom::Node::AsChecked<const HTMLCanvasElement>(node());
    formattingContext().setContentSize(canvas_element.width(),
                                       canvas_element.height());
    return true;
  }

  void LayoutHTMLCanvas::setDrawingBitmap(std::shared_ptr<const SkBitmap> src_bitmap)
  {
    adjustDrawingSize();

    auto setBitmap = [this, &src_bitmap](Scene &scene)
    {
      Image2d &imageComponent = scene.getComponentChecked<Image2d>(entity());
      // TODO(yorkie): use const in Image2d?
      imageComponent.bitmap = const_pointer_cast<SkBitmap>(src_bitmap);
      imageComponent.setVisible(true);
    };
    useSceneWithCallback(setBitmap);
    markCanvasAsDirty(); // Mark the canvas as dirty after setting the bitmap.
  }

  void LayoutHTMLCanvas::markCanvasAsDirty()
  {
    auto markAsDirty = [this](Scene &scene)
    {
      WebContent &webContent = scene.getComponentChecked<WebContent>(entity());
      webContent.setDirty(true);
    };
    useSceneWithCallback(markAsDirty);
  }

  void LayoutHTMLCanvas::entityDidCreate(ecs::EntityId entity)
  {
    LayoutReplaced::entityDidCreate(entity);

    auto addImageComponent = [&entity](Scene &scene)
    {
      scene.addComponent(entity, Image2d("", nullptr));
    };
    useSceneWithCallback(addImageComponent);
  }

  void LayoutHTMLCanvas::entityWillBeDestroyed(ecs::EntityId entity)
  {
    auto removeImageComponent = [&entity](Scene &scene)
    {
      scene.removeComponent<Image2d>(entity);
    };
    useSceneWithCallback(removeImageComponent);

    LayoutReplaced::entityWillBeDestroyed(entity);
  }
}
