#include "./animation_timeline.hpp"

namespace dom
{
  void AnimationTimeline::animationAttached(std::shared_ptr<Animation> animation)
  {
    if (TR_UNLIKELY(animation == nullptr))
      return;

    // TODO(yorkie): Check if the animation is already attached.
    animations_.push_back(animation);
  }

  void AnimationTimeline::animationDetached(std::shared_ptr<Animation> target_animation)
  {
    if (TR_UNLIKELY(target_animation == nullptr))
      return;

    for (auto it = animations_.begin(); it != animations_.end(); ++it)
    {
      auto animation_ref = *it;
      if (TR_UNLIKELY(animation_ref.expired()))
        continue;

      auto animation = animation_ref.lock();
      if (animation == target_animation)
      {
        animations_.erase(it);
        return;
      }
    }
  }

  void AnimationTimeline::detachInvalidAnimations()
  {
    for (auto it = animations_.begin(); it != animations_.end();)
    {
      auto animation = *it;
      if (animation.expired())
        it = animations_.erase(it);
      else
        it++;
    }
  }

  void AnimationTimeline::serviceAnimations()
  {
    // TODO(yorkie): implement this.
  }
}
