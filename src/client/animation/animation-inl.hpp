#pragma once

#include <memory>

#include "./animation.hpp"
#include "./animation_timeline.hpp"

namespace dom
{
  template <typename T>
  std::shared_ptr<T> Animation::MakeAnimation(std::unique_ptr<AnimationEffect> effect,
                                              std::shared_ptr<AnimationTimeline> timeline)
  {
    if (TR_UNLIKELY(timeline == nullptr))
      return nullptr;

    auto deleter = [](T *animation)
    {
      if (TR_LIKELY(animation != nullptr))
        delete animation;
    };
    auto animation = std::shared_ptr<T>(new T(std::move(effect), timeline), deleter);
    timeline->animationAttached(animation);
    return animation;
  }
}
