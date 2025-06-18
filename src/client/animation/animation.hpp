#pragma once

#include <memory>
#include <optional>
#include <string>
#include <client/cssom/computed_style.hpp>
#include "./animation_effect.hpp"

namespace dom
{
  class AnimationTimeline;
  class Element;

  class Animation : public std::enable_shared_from_this<Animation>
  {
  public:
    // Construct a new animation with specific type `T`.
    template <typename T>
    static std::shared_ptr<T> MakeAnimation(std::unique_ptr<AnimationEffect>, std::shared_ptr<AnimationTimeline>);

  public:
    Animation(std::unique_ptr<AnimationEffect>, std::shared_ptr<AnimationTimeline>);
    virtual ~Animation();

  public:
    virtual bool isCSSAnimation() const
    {
      return false;
    }
    virtual bool isCSSTransition() const
    {
      return false;
    }
    virtual std::shared_ptr<Element> owningElement() const
    {
      return nullptr;
    }
    virtual void clearOwningElement()
    {
    }
    inline bool isOwned() const
    {
      return owningElement() != nullptr;
    }

    // Returns whether the animation is finished.
    bool update(TimingUpdateReason);

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
    std::optional<float> currentTime() const;
    void setCurrentTime(float time);

    std::optional<float> startTime() const
    {
      return start_time_;
    }

    const AnimationEffect &effect() const;
    AnimationEffect &effect();

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

    float playbackRate() const
    {
      return playback_rate_;
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

  private:
    std::optional<float> calculateStartTime(float current_time) const;
    std::optional<float> calculateCurrentTime() const;

  private:
    std::string id_;
    std::optional<float> start_time_;
    std::optional<float> hold_time_;
    float playback_rate_ = 1.0f;

    std::unique_ptr<AnimationEffect> effect_;
    std::weak_ptr<AnimationTimeline> timeline_;
    bool ready_ = false;
    bool pending_ = false;
    PlayState play_state_ = kPlayStateIdle;
    ReplaceState replace_state_ = kReplaceStateActive;
  };
}
