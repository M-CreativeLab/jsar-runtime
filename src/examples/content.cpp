#include "./content.hpp"
#include "./xr_renderer.hpp"
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
    windowCtx_ = windowCtx;
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
    if (!isDragging_ || !contentRuntime_ || !windowCtx_)
      return;

    // Calculate the mouse movement delta in screen space (pixels)
    glm::vec2 mouseDelta = mousePosition - dragStartMousePos_;

    // Get viewport dimensions
    auto viewport = windowCtx_->drawingViewport();
    float viewportHeight = static_cast<float>(viewport.height());

    // Get camera parameters from XR renderer
    auto xrRenderer = windowCtx_->xrRenderer;
    if (!xrRenderer)
      return;

    // Get content position in world space
    glm::vec3 contentPosition = glm::vec3(dragStartContentMatrix_[3]);

    // Get camera position in world space
    glm::vec3 cameraPosition = xrRenderer->viewerPosition();

    // Calculate distance from camera to content
    float distance = glm::length(contentPosition - cameraPosition);

    // Use the camera's FOV (60 degrees by default in xr_renderer.hpp)
    float fov = 60.0f; // This matches the default in XRStereoscopicRenderer
    float fovInRadians = glm::radians(fov);
    float tanHalfFov = tan(fovInRadians / 2.0f);

    // Calculate the scale factor based on perspective projection
    // This ensures 1 pixel of mouse movement = 1 unit of world movement at the content's depth
    float worldUnitsPerPixel = (2.0f * tanHalfFov * distance) / viewportHeight;

    // Convert screen space delta to world space delta
    // Note: Y is inverted because screen Y goes down, world Y goes up
    glm::vec3 worldDelta = glm::vec3(
      mouseDelta.x * worldUnitsPerPixel,
      -mouseDelta.y * worldUnitsPerPixel,
      0.0f);

    // Apply camera rotation to the delta (so drag follows screen orientation)
    // For now, we only apply Y-axis rotation since the camera typically doesn't roll
    glm::mat4 viewerMatrix = xrRenderer->getViewerBaseMatrix();
    glm::mat3 viewerRotation = glm::mat3(viewerMatrix);
    // Only use the XZ plane rotation (ignore pitch)
    glm::vec3 right = glm::normalize(glm::vec3(viewerRotation[0].x, 0.0f, viewerRotation[0].z));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 rotatedDelta = right * worldDelta.x + up * worldDelta.y;

    // Create a new matrix with the updated position
    glm::mat4 newMatrix = dragStartContentMatrix_;
    newMatrix[3] += glm::vec4(rotatedDelta, 0.0f); // Update translation component

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

  void Content::processInput()
  {
    if (barComponent_)
    {
      barComponent_->processInput(this);
    }
  }
}