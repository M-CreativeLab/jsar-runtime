#include "./animation_timeline.hpp"

namespace endor
{
  namespace dom
  {
    using namespace std;

    void AnimationTimeline::animationAttached(shared_ptr<Animation> animation)
    {
      if (TR_UNLIKELY(animation == nullptr))
        return;

      // TODO(yorkie): Check if the animation is already attached.
      animations_.push_back(animation);
    }

    void AnimationTimeline::animationDetached(shared_ptr<Animation> target_animation)
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

    // This implements https://www.w3.org/TR/web-animations-1/#update-animations-and-send-events
    void AnimationTimeline::serviceAnimations(TimingUpdateReason reason)
    {
      updateCurrentTime();

      vector<shared_ptr<Animation>> animations;
      for (auto &animation : animations_)
      {
        if (TR_LIKELY(!animation.expired()))
          animations.push_back(animation.lock());
      }

      for (const auto &animation : animations)
      {
        if (animation->update(reason))
        {
          // TODO(yorkie): Dispatch events for the animation.
        }
      }
    }
  }
} // namespace endor
