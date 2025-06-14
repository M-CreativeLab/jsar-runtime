#pragma once

#include <vector>
#include "./keyframe.hpp"

namespace dom
{
  class Keyframes : std::vector<Keyframe>
  {
  public:
    static Keyframes Empty()
    {
      return Keyframes();
    }

  public:
    Keyframes()
        : std::vector<Keyframe>()
    {
    }

  public:
    void empty()
    {
      std::vector<Keyframe>::clear();
    }
  };
}
