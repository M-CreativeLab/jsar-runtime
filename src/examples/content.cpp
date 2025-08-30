#include "./content.hpp"
#include "./bar_component.hpp"

namespace jsar::example
{
  Content::Content(std::shared_ptr<TrContentRuntime> contentRuntime, uint32_t id)
      : id_(id)
      , contentRuntime_(contentRuntime)
      , centerPosition_(0.0f, 0.0f, 0.35f) // Default position
      , isDragging_(false)
      , dragStartMousePos_(0.0f)
      , dragStartContentPos_(0.0f)
  {
    // Note: BarComponent will be created when we have a WindowContext available
  }

  void Content::setWindowContext(WindowContext *windowCtx)
  {
    if (!barComponent_ && windowCtx)
    {
      barComponent_ = std::make_shared<BarComponent>(windowCtx, this);
      barComponent_->updatePosition(centerPosition_);
    }
  }

  Content::~Content()
  {
  }

  void Content::setCenterPosition(const glm::vec3 &position)
  {
    centerPosition_ = position;
    if (barComponent_)
    {
      barComponent_->updatePosition(centerPosition_);
    }
  }

  void Content::startDrag(const glm::vec2 &mousePosition)
  {
    isDragging_ = true;
    dragStartMousePos_ = mousePosition;
    dragStartContentPos_ = centerPosition_;

    if (barComponent_)
    {
      barComponent_->setDragging(true);
    }
  }

  void Content::updateDrag(const glm::vec2 &mousePosition)
  {
    if (!isDragging_)
      return;

    // Calculate the mouse movement delta
    glm::vec2 mouseDelta = mousePosition - dragStartMousePos_;

    // Convert screen space movement to world space movement
    // For now, we'll use a simple scale factor - this could be improved
    // by using proper screen-to-world coordinate transformation
    float scale = 0.001f; // Adjust this value to control drag sensitivity

    glm::vec3 worldDelta = glm::vec3(mouseDelta.x * scale, -mouseDelta.y * scale, 0.0f);
    setCenterPosition(dragStartContentPos_ + worldDelta);
  }

  void Content::stopDrag()
  {
    isDragging_ = false;

    if (barComponent_)
    {
      barComponent_->setDragging(false);
    }
  }

  void Content::update()
  {
    // Update any animation or state for this content
    if (barComponent_)
    {
      barComponent_->updatePosition(centerPosition_);
    }
  }

  bool Content::isPointInBar(const glm::vec2 &screenPosition) const
  {
    if (!barComponent_)
      return false;

    return barComponent_->isPointInBounds(screenPosition);
  }
}