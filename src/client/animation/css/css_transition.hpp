#pragma once

#include <client/animation/animation.hpp>

namespace dom
{
  class CSSTransition : public Animation
  {
    using Animation::Animation;

  public:
    bool isCSSTransition() const override { return true; }
  };
}
