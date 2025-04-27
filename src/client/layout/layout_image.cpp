#include <optional>
#include <client/builtin_scene/scene.hpp>
#include <client/dom/node.hpp>
#include <client/html/html_image_element.hpp>

#include "./layout_image.hpp"
#include "./layout_view.hpp"

namespace client_layout
{
  using namespace std;
  using namespace dom;
  using namespace builtin_scene;
  using namespace crates::layout2::styles;

  bool LayoutImage::adjustImageSize()
  {
    const auto &imageElement = dom::Node::AsChecked<const dom::HTMLImageElement>(node());
    const auto &adoptedStyle = imageElement.adoptedStyleRef();
    const auto srcImageRect = imageElement.getImageClientRect();
    const auto lastFragment = fragment();

    // Not adjusting the size if the image source is not loaded yet.
    if (srcImageRect.width() <= 0 || srcImageRect.height() <= 0)
      return false;

    optional<Dimension> adoptedWidth = nullopt;
    optional<Dimension> adoptedHeight = nullopt;

    if (adoptedStyle.hasProperty("width"))
    {
      auto widthDimension = adoptedStyle.getPropertyValueAs<Dimension>("width");
      if (!widthDimension.isAuto() &&
          lastFragment.contentWidth() > 0)
        adoptedWidth = widthDimension;
    }
    if (adoptedStyle.hasProperty("height"))
    {
      auto heightDimension = adoptedStyle.getPropertyValueAs<Dimension>("height");
      if (!heightDimension.isAuto() &&
          lastFragment.contentHeight() > 0)
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
      float adjustedHeight = lastFragment.contentWidth() / aspectRatio;
      formattingContext().setContentSize(lastFragment.contentWidth(), adjustedHeight);
      return true;
    }
    else if (!adoptedWidth.has_value() && adoptedHeight.has_value())
    {
      // Calculate the width = height * aspectRatio.
      float adjustedWidth = lastFragment.contentHeight() * aspectRatio;
      formattingContext().setContentSize(adjustedWidth, lastFragment.contentHeight());
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

  void LayoutImage::entityDidCreate(ecs::EntityId entity)
  {
    LayoutReplaced::entityDidCreate(entity);

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

  void LayoutImage::didComputeLayoutOnce(const ConstraintSpace &avilableSpace)
  {
    LayoutReplaced::didComputeLayoutOnce(avilableSpace);

    Fragment fragment = this->fragment();
    if (!is_layout_ready_)
    {
      layoutDidFirstReady(fragment);
      is_layout_ready_ = true;
    }

    bool shouldVisible = fragment.visibleInViewport(viewRef().viewport);
    if (shouldVisible)
      Node::AsChecked<dom::HTMLImageElement>(node()).loadImageAsync();

    setVisible(shouldVisible);
  }

  void LayoutImage::sizeDidChange()
  {
    LayoutReplaced::sizeDidChange();

    adjustImageSize();
  }

  void LayoutImage::layoutDidFirstReady(const Fragment &fragment)
  {
  }

  void LayoutImage::setVisible(bool b)
  {
    auto setVisible = [this, &b](Scene &scene)
    {
      Image2d &imageComponent = scene.getComponentChecked<Image2d>(entity());
      imageComponent.setVisible(b);
    };
    useSceneWithCallback(setVisible);

    // When the visibility is changed, we need to mark the content as dirty to update the texture altas.
    // This optimization is to remove the textures which are invisible from the texture atlas, it keeps the consistent
    // size of the texture atlas.
    if (!last_visible_.has_value() || last_visible_.value() != b)
    {
      auto markContentDirty = [this](Scene &scene)
      {
        WebContent &webContent = scene.getComponentChecked<WebContent>(entity());
        webContent.setDirty(true);
      };
      useSceneWithCallback(markContentDirty);
    }
    last_visible_ = b;
  }
}
