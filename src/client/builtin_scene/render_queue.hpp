#pragma once

#include <stdint.h>
#include <iostream>

namespace builtin_scene
{
  /**
   * RenderQueue is used to determine the rendering order of elements in the scene.
   */
  class RenderQueue
  {
  public:
    RenderQueue(uint32_t base = 0)
        : base(base)
        , zIndex(0)
        , translateZ(0)
    {
    }
    RenderQueue(const RenderQueue &) = default;
    ~RenderQueue() = default;

  public:
    bool operator<(const RenderQueue &other) const;
    bool operator>(const RenderQueue &other) const;
    bool operator==(const RenderQueue &other) const;
    bool operator!=(const RenderQueue &other) const;

    friend std::ostream &operator<<(std::ostream &os, const RenderQueue &queue)
    {
      os << "RenderQueue(base=" << queue.base
         << ", zIndex=" << queue.zIndex
         << ", translateZ=" << queue.translateZ
         << ")";
      return os;
    }

  public:
    // The base number of the render queue.
    uint32_t base;
    // CSS: z-index
    int zIndex = 0;
    // CSS: transform: translateZ()
    float translateZ = 0;
  };
}
