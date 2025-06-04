#include "./animation.hpp"

namespace dom
{
  using namespace std;

  Animation::Animation(unique_ptr<AnimationEffect> effect,
                       shared_ptr<AnimationTimeline> timeline)
      : effect_(move(effect)),
        timeline_(timeline),
        id_(""),
        pending_(false),
        ready_(false),
        play_state_(kPlayStateIdle),
        current_time_(0.0f),
        playbackRate(1.0f)
  {
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
}
