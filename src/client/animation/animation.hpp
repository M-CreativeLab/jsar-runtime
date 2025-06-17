#pragma once

#include <memory>
#include <string>
#include <client/cssom/computed_style.hpp>

namespace dom
{
  class AnimationEffect;
  class AnimationTimeline;

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
    std::weak_ptr<AnimationTimeline> timeline_;
    bool ready_ = false;
    bool pending_ = false;
    PlayState play_state_ = kPlayStateIdle;
    ReplaceState replace_state_ = kReplaceStateActive;
  };
}
