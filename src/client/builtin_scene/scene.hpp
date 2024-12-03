#pragma once

#include <memory>
#include "./hierarchy.hpp"
#include "../graphics/webgl_context.hpp"

namespace builtin_scene
{
  class Scene : public Hierarchy
  {
  public:
    Scene(std::shared_ptr<client_graphics::WebGL2Context> glContext)
        : Hierarchy(), glContext(glContext)
    {
      assert(glContext != nullptr);
    }
    ~Scene() = default;

  public:
    void update()
    {
    }

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext;
  };
}
