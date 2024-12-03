#pragma once

#include <vector>
#include <memory>
#include "./renderable_object.hpp"

namespace builtin_scene
{
  class Hierarchy : IRenderableObject
  {
  public:
    Hierarchy() : IRenderableObject() {}
    ~Hierarchy() = default;

  public:
    void addChild(std::shared_ptr<IRenderableObject> child)
    {
      children.push_back(child);
    }
    void removeChild(std::shared_ptr<IRenderableObject> child)
    {
      auto it = std::find(children.begin(), children.end(), child);
      if (it != children.end())
        children.erase(it);
    }

  protected:
    std::vector<std::shared_ptr<IRenderableObject>> children;
  };
}
