#include <iostream>
#include <algorithm>

#include "./bar_component.hpp"
#include "./content.hpp"

namespace jsar::example
{
  using namespace std;

  BarComponent::BarComponent()
  {
    // Create the canvas system first
    canvas_ = make_shared<CanvasSystem>();

    // Create the 3D component with reference to canvas system
    bar3d_ = make_shared<ContentBar3d>(canvas_);

    if (glGetError() != GL_NO_ERROR)
      cout << "OpenGL error on BarComponent init" << endl;
  }

  BarComponent::~BarComponent()
  {
    // Components will clean up themselves
  }

  void BarComponent::addContent(Content *content)
  {
    bar3d_->addContent(content);
  }

  void BarComponent::removeContent(Content *content)
  {
    bar3d_->removeContent(content);
  }

  void BarComponent::updateContentTransform(Content *content, const glm::mat4 &transform)
  {
    bar3d_->updateContentTransform(content, transform);
  }

  void BarComponent::setContentHovered(Content *content, bool hovered)
  {
    bar3d_->setContentHovered(content, hovered);
  }

  void BarComponent::setContentDragging(Content *content, bool dragging)
  {
    bar3d_->setContentDragging(content, dragging);
  }

  Content *BarComponent::checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const
  {
    return bar3d_->checkRayIntersection(rayOrigin, rayDirection);
  }
}