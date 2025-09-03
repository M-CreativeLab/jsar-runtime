#include <iostream>
#include <algorithm>

#include <common/command_buffers/webgl_constants.hpp>

#include "./content_bar_3d.hpp"
#include "./content.hpp"

namespace jsar::example
{
  using namespace std;
  using namespace builtin_scene;

  ContentBar3d::ContentBar3d(std::shared_ptr<CanvasSystem> canvas)
      : Mesh("ContentBar3d", PrimitiveTopology::TRIANGLES)
      , canvas_(canvas)
  {
    initializeMeshGeometry();

    // Register ray event handler with canvas system
    canvas_->registerRayEventHandler([this](const RayEvent &event) -> bool
                                     {
                                       processRayEvent(event);
                                       return true; // Handle all ray events
                                     });
  }

  ContentBar3d::~ContentBar3d()
  {
    // Mesh destructor will handle cleanup
  }

  float ContentBar3d::area()
  {
    return BAR_WIDTH * BAR_HEIGHT;
  }

  float ContentBar3d::volume()
  {
    return 0.0f; // 2D bar has no volume
  }

  void ContentBar3d::addContent(Content *content)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                      { return instance.content == content; });

    if (it == instances_.end())
    {
      instances_.emplace_back(content);
    }
  }

  void ContentBar3d::removeContent(Content *content)
  {
    instances_.erase(
      remove_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                { return instance.content == content; }),
      instances_.end());
  }

  void ContentBar3d::updateContentTransform(Content *content, const glm::mat4 &transform)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                      { return instance.content == content; });

    if (it != instances_.end())
    {
      it->transform = transform;
    }
  }

  Content *ContentBar3d::checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const
  {
    // Create ray event and process it
    RayEvent event;
    event.type = RayEventType::Move;
    event.rayOrigin = rayOrigin;
    event.rayDirection = rayDirection;

    // Simple ray-plane intersection for each bar instance
    for (const auto &instance : instances_)
    {
      // Calculate bar position from transform
      glm::vec3 barPosition = glm::vec3(instance.transform[3]);
      barPosition.y += BAR_OFFSET_Y;

      // Simple ray-plane intersection (assuming bar is on XZ plane)
      float t = (barPosition.y - rayOrigin.y) / rayDirection.y;
      if (t > 0)
      {
        glm::vec3 intersectionPoint = rayOrigin + t * rayDirection;

        // Check if intersection is within bar bounds
        float halfWidth = BAR_WIDTH * 0.5f;
        if (abs(intersectionPoint.x - barPosition.x) <= halfWidth &&
            abs(intersectionPoint.z - barPosition.z) <= halfWidth)
        {
          // Calculate local position for the event
          event.localPosition.x = (intersectionPoint.x - barPosition.x + halfWidth) / BAR_WIDTH;
          event.localPosition.y = (intersectionPoint.z - barPosition.z + halfWidth) / BAR_WIDTH;

          // Forward event to canvas system
          const_cast<CanvasSystem *>(canvas_.get())->processRayEvent(event);

          return instance.content;
        }
      }
    }

    return nullptr;
  }

  void ContentBar3d::setContentHovered(Content *content, bool hovered)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                      { return instance.content == content; });

    if (it != instances_.end())
    {
      it->isHovered = hovered;
      canvas_->updateContentState(content, hovered, it->isDragging);
    }
  }

  void ContentBar3d::setContentDragging(Content *content, bool dragging)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                      { return instance.content == content; });

    if (it != instances_.end())
    {
      it->isDragging = dragging;
      canvas_->updateContentState(content, it->isHovered, dragging);
    }
  }

  void ContentBar3d::initializeMeshGeometry()
  {
    // Create a simple quad for the bar
    float halfWidth = BAR_WIDTH * 0.5f;
    float halfHeight = BAR_HEIGHT * 0.5f;

    // Define vertices for a quad (position + UV coordinates)
    Vertex v1(glm::vec3(-halfWidth, -halfHeight, 0), glm::vec3(0, 1, 0), glm::vec2(0, 0));
    Vertex v2(glm::vec3(halfWidth, -halfHeight, 0), glm::vec3(0, 1, 0), glm::vec2(1, 0));
    Vertex v3(glm::vec3(halfWidth, halfHeight, 0), glm::vec3(0, 1, 0), glm::vec2(1, 1));
    Vertex v4(glm::vec3(-halfWidth, halfHeight, 0), glm::vec3(0, 1, 0), glm::vec2(0, 1));

    // Add vertices to mesh
    insertVertex(v1);
    insertVertex(v2);
    insertVertex(v3);
    insertVertex(v4);

    // Define indices for two triangles forming a quad
    Indices<uint32_t> indices = {0, 1, 2, 0, 2, 3};
    updateIndices(indices);

    // Enable required vertex attributes
    enableAttribute(Vertex::ATTRIBUTE_POSITION);
    enableAttribute(Vertex::ATTRIBUTE_NORMAL);
    enableAttribute(Vertex::ATTRIBUTE_UV0);
  }

  void ContentBar3d::processRayEvent(const RayEvent &event)
  {
    // Process ray events forwarded from canvas system
    // This could trigger additional 3D-specific behaviors
    switch (event.type)
    {
    case RayEventType::Down:
      // Handle ray down event
      break;
    case RayEventType::Up:
      // Handle ray up event
      break;
    case RayEventType::Move:
      // Handle ray move event
      break;
    }
  }

  glm::mat4 ContentBar3d::calculateBarTransform(const glm::vec3 &contentPosition) const
  {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, contentPosition + glm::vec3(0, BAR_OFFSET_Y, 0));
    return transform;
  }
}