#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <common/viewport.hpp>
#include "./ecs.hpp"
#include "./transform.hpp"
#include "./xr.hpp"

namespace builtin_scene
{
  class Camera : public ecs::Component
  {
  public:
    using ecs::Component::Component;

  public:
    bool active()
    {
      return active_;
    }

  private:
    bool active_ = false;
    std::optional<TrViewport> viewport_ = std::nullopt;
  };

  class CameraStartupSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    const std::string name() const override
    {
      return "CameraStartupSystem";
    }
    void onExecute() override
    {
      // Create the camera for rendering.
      spawn(Camera(), Transform());
    }
  };

  class CameraUpdateSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    const std::string name() const override
    {
      return "CameraUpdateSystem";
    }
    void onExecute() override
    {
      auto cameraEntity = firstEntity<Camera>();
      if (!cameraEntity.has_value())
        throw std::runtime_error("Camera not found.");

      auto camera = getComponent<Camera>(cameraEntity.value());
      if (camera != nullptr && camera->active())
      {
        // Update and render the camera.
      }
    }
  };
}
