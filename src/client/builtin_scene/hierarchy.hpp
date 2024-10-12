#pragma once

#include <vector>
#include <memory>
#include "./renderable_object.hpp"

namespace builtin_scene
{
  class Hierarchy : IRenderableObject
  {
  public:
    Hierarchy();
    ~Hierarchy();

  public:
    void addChild(std::shared_ptr<IRenderableObject> child);
    void removeChild(std::shared_ptr<IRenderableObject> child);

  protected:
    std::vector<std::shared_ptr<IRenderableObject>> children;
  };
}
