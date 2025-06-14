#pragma once

#include <memory>
#include <string>
#include <client/cssom/computed_style.hpp>

#include "./animation_effect.hpp"
#include "./animation_timeline.hpp"

namespace dom
{
  class Animation
  {
  public:
    Animation(std::unique_ptr<AnimationEffect>, std::shared_ptr<const AnimationTimeline>);
    virtual ~Animation() = default;

  public:
    virtual bool isCSSAnimation() const
    {
      return false;
    }
    virtual bool isCSSTransition() const
    {
      return false;
    }

    void cancel();
    void commitStyles();
    void finish();
    void pause();
    void persist();
    void play();
    void reverse();
    void updatePlaybackRate(float);

    bool updateFrameToStyle(client_cssom::ComputedStyle &);
    bool updatePropertyToStyle(client_cssom::ComputedStyle &, const std::string &property);

  public:
    std::optional<float> currentTime() const
    {
      if (!played() ||
          timeline_.expired() ||
          !timeline_.lock()->isActive())
        return std::nullopt;
      else
        return current_time_;
    }
    void setCurrentTime(float time)
    {
      current_time_ = time;
    }

    const AnimationEffect &effect() const
    {
      return *effect_;
    }
    AnimationEffect &effect()
    {
      return *effect_;
    }

    std::string id() const
    {
      return id_;
    }
    bool ready() const
    {
      return ready_;
    }

    enum PlayState
    {
      kPlayStateIdle,
      kPlayStateRunning,
      kPlayStatePaused,
      kPlayStateFinished,
    };
    bool pending() const
    {
      return pending_;
    }
    bool played() const
    {
      return play_state_ == kPlayStateRunning ||
             play_state_ == kPlayStatePaused;
    }
    PlayState playState() const
    {
      return play_state_;
    }

    enum ReplaceState
    {
      kReplaceStateActive,
      kReplaceStatePersisted,
      kReplaceStateRemoved,
    };
    ReplaceState replaceState() const
    {
      return replace_state_;
    }

  public:
    float playbackRate;
    float startTime;

  private:
    std::string id_;
    float current_time_ = 0;
    std::unique_ptr<AnimationEffect> effect_;
    std::weak_ptr<const AnimationTimeline> timeline_;
    bool ready_ = false;
    bool pending_ = false;
    PlayState play_state_ = kPlayStateIdle;
    ReplaceState replace_state_ = kReplaceStateActive;
  };
}
