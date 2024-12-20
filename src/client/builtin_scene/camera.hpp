#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <common/viewport.hpp>
#include "./ecs.hpp"

namespace builtin_scene
{
  class Camera : public ecs::Component
  {
  public:
    Camera()
    {
    }
    ~Camera() = default;

  private:
    bool isActive_ = false;
    std::optional<TrViewport> viewport_ = std::nullopt;
  };

  class CameraSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    void onExecute()
    {
    }
  };
}
