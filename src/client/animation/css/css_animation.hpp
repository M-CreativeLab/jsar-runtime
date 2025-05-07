#pragma once

#include <client/animation/animation.hpp>

namespace dom
{
  class CSSAnimation : public Animation
  {
  public:
    bool isCSSAnimation() const override { return true; }
  };
}