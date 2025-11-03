#pragma once

#include <optional>
#include <chrono>
#include <vector>
#include <memory>

#include "./animation.hpp"

namespace endor
{
  namespace dom
  {
    class AnimationTimeline
    {
    public:
      AnimationTimeline() = default;
      virtual ~AnimationTimeline() = default;

      virtual bool isDocumentTimeline() const
      {
        return false;
      }
      virtual bool isScrollTimeline() const
      {
        return false;
      }
      virtual bool isViewTimeline() const
      {
        return false;
      }
      virtual bool isActive() const
      {
        return current_time_ != std::nullopt;
      }
      virtual bool isResolved() const
      {
        return true;
      }

      std::optional<long long> currentTime() const
      {
        if (!isActive())
          return std::nullopt;
        return std::chrono::duration_cast<std::chrono::milliseconds>(current_time_->time_since_epoch()).count();
      }

      virtual void animationAttached(std::shared_ptr<Animation>);
      virtual void animationDetached(std::shared_ptr<Animation>);
      virtual void detachInvalidAnimations();

      virtual void serviceAnimations(TimingUpdateReason);
      virtual void scheduleNextService() = 0;

      virtual bool hasAnimations() const
      {
        return !animations_.empty();
      }

    protected:
      void updateCurrentTime()
      {
        current_time_ = std::chrono::high_resolution_clock::now();
      }

    protected:
      std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> current_time_;

    private:
      std::vector<std::weak_ptr<Animation>> animations_;
    };
  }
} // namespace endor
