#include "./animation-inl.hpp"
#include "./animation_effect.hpp"
#include "./animation_timeline.hpp"

namespace dom
{
  using namespace std;

  Animation::Animation(unique_ptr<AnimationEffect> effect,
                       shared_ptr<AnimationTimeline> timeline)
      : effect_(move(effect))
      , timeline_(timeline)
      , id_("")
      , pending_(false)
      , ready_(false)
      , play_state_(kPlayStateIdle)
      , playback_rate_(1.0f)
  {
  }

  Animation::~Animation()
  {
    if (!timeline_.expired())
    {
      auto timeline = timeline_.lock();
      if (timeline)
        timeline->detachInvalidAnimations();
    }
  }

  bool Animation::update(TimingUpdateReason reason)
  {
    // TODO(yorkie): update the animation timings, and mark the target elements to be needed to recalculate styles.
    return true;
  }

  void Animation::cancel()
  {
    play_state_ = kPlayStateIdle;
    pending_ = false;
    ready_ = false;

    // TODO: clear all effects
    // TODO: abort playback
  }

  void Animation::commitStyles()
  {
    // TODO: commit animation styles to target element's style
  }

  void Animation::finish()
  {
    // TODO: finish playback of the animation
  }

  void Animation::pause()
  {
    // TODO: suspends playback of the animation
  }

  void Animation::persist()
  {
    replace_state_ = ReplaceState::kReplaceStatePersisted;
  }

  void Animation::play()
  {
    play_state_ = kPlayStateRunning;
  }

  void Animation::reverse()
  {
  }

  void Animation::updatePlaybackRate(float playback_rate)
  {
    playback_rate_ = playback_rate;
  }

  bool Animation::updateFrameToStyle(client_cssom::ComputedStyle &)
  {
    // TODO(yorkie): update the animation frame to the style.
    return false;
  }

  bool Animation::updatePropertyToStyle(client_cssom::ComputedStyle &style, const std::string &property)
  {
    // Skip if the timeline is not active or expired.
    if (TR_UNLIKELY(timeline_.expired() || !timeline_.lock()->isActive()))
      return false;

    // Skip if the property is not animatable or not set in the style.
    if (!style.hasProperty(property))
      return false;

    // Switches the playing state based on the current.
    if (play_state_ == kPlayStateIdle ||
        play_state_ == kPlayStateFinished)
    {
      play();
    }
    else if (play_state_ == kPlayStatePaused)
    {
      play_state_ = kPlayStateRunning;
    }

    // TODO(yorkie): update the property based on the animation effect.
    return false;
  }

  std::optional<float> Animation::currentTime() const
  {
    return hold_time_ ? hold_time_ : calculateCurrentTime();
  }

  void Animation::setCurrentTime(float time)
  {
    if (hold_time_ || !start_time_ || timeline_.expired() || !timeline_.lock()->isActive() ||
        playback_rate_ == 0.0f)
    {
      hold_time_ = time;
    }
    else
    {
      start_time_ = calculateStartTime(time);
    }
  }

  const AnimationEffect &Animation::effect() const
  {
    return *effect_;
  }

  AnimationEffect &Animation::effect()
  {
    return *effect_;
  }

  optional<float> Animation::calculateStartTime(float current_time) const
  {
    optional<float> start_time;
    if (timeline_.expired())
    {
      auto timeline_time = timeline_.lock()->currentTime();
      if (timeline_time.has_value())
        start_time = timeline_time.value() - current_time / playback_rate_;
    }
    return start_time;
  }

  optional<float> Animation::calculateCurrentTime() const
  {
    if (!start_time_.has_value() || timeline_.expired())
      return std::nullopt;
    auto timeline = timeline_.lock();
    if (!timeline->isActive())
      return std::nullopt;

    auto timeline_time = timeline->currentTime();
    assert(timeline_time.has_value() && "The timeline time should be valid.");
    return (timeline_time.value() - start_time_.value()) * playback_rate_;
  }
}
