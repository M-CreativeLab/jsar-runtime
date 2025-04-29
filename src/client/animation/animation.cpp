#include "./animation.hpp"

namespace dom
{
  Animation::Animation(const AnimationEffect &, std::shared_ptr<AnimationTimeline>)
  {
  }

  void Animation::cancel()
  {
  }

  void Animation::commitStyles()
  {
  }

  void Animation::finish()
  {
  }

  void Animation::pause()
  {
  }

  void Animation::persist()
  {
  }

  void Animation::play()
  {
  }

  void Animation::reverse()
  {
  }

  void Animation::updatePlaybackRate(float playback_rate)
  {
    playbackRate = playback_rate;
  }
}
