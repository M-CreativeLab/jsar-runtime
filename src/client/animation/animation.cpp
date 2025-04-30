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
}
