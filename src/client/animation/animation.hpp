#pragma once

#include <memory>
#include <string>

#include "./animation_effect.hpp"
#include "./animation_timeline.hpp"

namespace dom
{
  class Animation
  {
  public:
    Animation() = default;
    Animation(const AnimationEffect &, std::shared_ptr<AnimationTimeline>);

  public:
    void cancel();
    void commitStyles();
    void finish();
    void pause();
    void persist();
    void play();
    void reverse();
    void updatePlaybackRate(float);

  public:
    float currentTime() const { return current_time_; }

    const AnimationEffect &effect() const { return *effect_; }
    AnimationEffect &effect() { return *effect_; }

    std::string id() const { return id_; }
    bool ready() const { return ready_; }

    enum PlayState
    {
      kPlayStateIdle,
      kPlayStateRunning,
      kPlayStatePaused,
      kPlayStateFinished,
    };
    bool pending() const { return pending_; }
    PlayState playState() const { return play_state_; }

    enum ReplaceState
    {
      kReplaceStateActive,
      kReplaceStatePersisted,
      kReplaceStateRemoved,
    };
    ReplaceState replaceState() const { return replace_state_; }

  public:
    float playbackRate;
    float startTime;
    std::weak_ptr<AnimationTimeline> timeline;

  private:
    std::string id_;
    float current_time_ = 0;
    std::unique_ptr<AnimationEffect> effect_;
    bool ready_ = false;
    bool pending_ = false;
    PlayState play_state_ = kPlayStateIdle;
    ReplaceState replace_state_ = kReplaceStateActive;
  };
}
