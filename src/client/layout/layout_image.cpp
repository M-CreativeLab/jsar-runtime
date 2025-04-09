#include <optional>
#include <client/builtin_scene/scene.hpp>
#include <client/dom/node.hpp>
#include <client/dom/html_image_element.hpp>

#include "./layout_image.hpp"

namespace client_layout
{
  using namespace std;
  using namespace builtin_scene;
  using namespace crates::layout2::styles;

  bool LayoutImage::adjustImageSize()
  {
    auto imageElement = dom::Node::AsChecked<dom::HTMLImageElement>(node());
    auto &adoptedStyle = imageElement.adoptedStyle();
    auto srcImageRect = imageElement.getImageClientRect();
    auto lastFragment = fragment();

    optional<Dimension> adoptedWidth = nullopt;
    optional<Dimension> adoptedHeight = nullopt;

    if (adoptedStyle.hasProperty("width"))
    {
      auto widthDimension = adoptedStyle.getPropertyValueAs<Dimension>("width");
      if (!widthDimension.isAuto() &&
          lastFragment.width() > 0)
        adoptedWidth = widthDimension;
    }
    if (adoptedStyle.hasProperty("height"))
    {
      auto heightDimension = adoptedStyle.getPropertyValueAs<Dimension>("height");
      if (!heightDimension.isAuto() &&
          lastFragment.height() > 0)
        adoptedHeight = heightDimension;
    }

    // If both width and height are specified, then use them.
    if (adoptedWidth.has_value() && adoptedHeight.has_value())
      return false;

    // If both width and height are auto, then use the image's size.
    if (!adoptedWidth.has_value() && !adoptedHeight.has_value())
    {
      formattingContext().setContentSize(srcImageRect.width(),
                                         srcImageRect.height());
      return true;
    }

    const float aspectRatio = srcImageRect.width() / srcImageRect.height();
    if (adoptedWidth.has_value() && !adoptedHeight.has_value())
    {
      // Calculate the height = width / aspectRatio.
      formattingContext().setContentHeight(lastFragment.width() / aspectRatio);
      return true;
    }
    else if (!adoptedWidth.has_value() && adoptedHeight.has_value())
    {
      // Calculate the width = height * aspectRatio.
      formattingContext().setContentWidth(lastFragment.height() * aspectRatio);
      return true;
    }

    assert(false && "Unreachable");
    return false;
  }

  void LayoutImage::setImageBitmap(std::shared_ptr<SkBitmap> srcBitmap)
  {
    adjustImageSize();

    auto setBitmap = [this, &srcBitmap](Scene &scene)
    {
      Image2d &imageComponent = scene.getComponentChecked<Image2d>(entity());
      imageComponent.bitmap = srcBitmap;

      WebContent &webContent = scene.getComponentChecked<WebContent>(entity());
      webContent.setDirty(true); // Mark the content as dirty to update the texture.
    };
    useSceneWithCallback(setBitmap);
  }

  void LayoutImage::entityDidCreated(ecs::EntityId entity)
  {
    LayoutReplaced::entityDidCreated(entity);

    auto addImageComponent = [this, &entity](Scene &scene)
    {
      assert(dom::Node::Is<dom::HTMLImageElement>(node()));
      auto &imageElement = dom::Node::AsChecked<dom::HTMLImageElement>(node());
      scene.addComponent(entity, Image2d(imageElement.getSrc(), nullptr));
    };
    useSceneWithCallback(addImageComponent);
  }

  void LayoutImage::entityWillBeDestroyed(ecs::EntityId entity)
  {
    auto removeImageComponent = [&entity](Scene &scene)
    {
      scene.removeComponent<Image2d>(entity);
    };
    useSceneWithCallback(removeImageComponent);

    LayoutReplaced::entityWillBeDestroyed(entity);
  }

  void LayoutImage::sizeDidChanged()
  {
    LayoutReplaced::sizeDidChanged();

    adjustImageSize();
  }
}
