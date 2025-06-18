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
      , current_time_(0.0f)
      , playbackRate(1.0f)
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

  bool Animation::update()
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
    current_time_ = 0.0f;
  }

  void Animation::reverse()
  {
  }

  void Animation::updatePlaybackRate(float playback_rate)
  {
    playbackRate = playback_rate;
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
    if (!played() ||
        timeline_.expired() ||
        !timeline_.lock()->isActive())
      return std::nullopt;
    else
      return current_time_;
  }

  void Animation::setCurrentTime(float time)
  {
    current_time_ = time;
  }

  const AnimationEffect &Animation::effect() const
  {
    return *effect_;
  }

  AnimationEffect &Animation::effect()
  {
    return *effect_;
  }
}
