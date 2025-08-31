#include "./content.hpp"
#include "./bar_component.hpp"

namespace jsar::example
{
  Content::Content(std::shared_ptr<TrContentRuntime> contentRuntime, uint32_t id)
      : id_(id)
      , contentRuntime_(contentRuntime)
      , isDragging_(false)
      , dragStartMousePos_(0.0f)
      , dragStartContentMatrix_(1.0f)
  {
    // Note: BarComponent will be created when we have a WindowContext available
  }

  void Content::setWindowContext(WindowContext *windowCtx)
  {
    // Note: Bar component is now shared across all content instances
    // This method is kept for compatibility but bar creation is handled externally
  }

  Content::~Content()
  {
  }

  glm::vec3 Content::getCenterPosition() const
  {
    if (!contentRuntime_)
      return glm::vec3(0.0f);

    auto activeSession = contentRuntime_->getActiveXRSession();
    if (!activeSession)
      return glm::vec3(0.0f);

    glm::mat4 baseMatrix = activeSession->getLocalBaseMatrix();
    return glm::vec3(baseMatrix[3]); // Extract translation from matrix
  }

  glm::quat Content::getRotation() const
  {
    if (!contentRuntime_)
      return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    auto activeSession = contentRuntime_->getActiveXRSession();
    if (!activeSession)
      return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::mat4 baseMatrix = activeSession->getLocalBaseMatrix();
    // Extract rotation from the upper-left 3x3 part of the matrix
    glm::mat3 rotationMatrix = glm::mat3(baseMatrix);
    return glm::quat_cast(rotationMatrix);
  }

  glm::vec3 Content::getScaling() const
  {
    if (!contentRuntime_)
      return glm::vec3(1.0f);

    auto activeSession = contentRuntime_->getActiveXRSession();
    if (!activeSession)
      return glm::vec3(1.0f);

    glm::mat4 baseMatrix = activeSession->getLocalBaseMatrix();
    // Extract scaling from the matrix
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(baseMatrix[0]));
    scale.y = glm::length(glm::vec3(baseMatrix[1]));
    scale.z = glm::length(glm::vec3(baseMatrix[2]));
    return scale;
  }

  void Content::startDrag(const glm::vec2 &mousePosition)
  {
    isDragging_ = true;
    dragStartMousePos_ = mousePosition;

    // Store the current base matrix instead of just position
    if (contentRuntime_)
    {
      auto activeSession = contentRuntime_->getActiveXRSession();
      if (activeSession)
      {
        dragStartContentMatrix_ = activeSession->getLocalBaseMatrix();
      }
    }

    if (barComponent_)
    {
      barComponent_->setContentDragging(this, true);
    }
  }

  void Content::updateDrag(const glm::vec2 &mousePosition)
  {
    if (!isDragging_ || !contentRuntime_)
      return;

    // Calculate the mouse movement delta
    glm::vec2 mouseDelta = mousePosition - dragStartMousePos_;

    // Convert screen space movement to world space movement
    // For now, we'll use a simple scale factor - this could be improved
    // by using proper screen-to-world coordinate transformation
    float scale = 0.001f; // Adjust this value to control drag sensitivity

    glm::vec3 worldDelta = glm::vec3(mouseDelta.x * scale, -mouseDelta.y * scale, 0.0f);

    // Create a new matrix with the updated position
    glm::mat4 newMatrix = dragStartContentMatrix_;
    newMatrix[3] += glm::vec4(worldDelta, 0.0f); // Update translation component

    // Update the content runtime's base matrix
    contentRuntime_->updateLocalBaseMatrix(newMatrix);
  }

  void Content::stopDrag()
  {
    isDragging_ = false;

    if (barComponent_)
    {
      barComponent_->setContentDragging(this, false);
    }
  }

  void Content::update()
  {
    // Update the bar component with new transform if needed
    if (barComponent_)
    {
      if (contentRuntime_)
      {
        auto activeSession = contentRuntime_->getActiveXRSession();
        if (activeSession)
        {
          barComponent_->updateContentTransform(this, activeSession->getLocalBaseMatrix());
        }
      }
    }
  }

  bool Content::isRayInBar(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const
  {
    if (!barComponent_)
      return false;

    Content *hitContent = barComponent_->checkRayIntersection(rayOrigin, rayDirection);
    return hitContent == this;
  }
}