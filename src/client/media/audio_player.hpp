#pragma once

#include <string>
#include "./media_player.hpp"

namespace media_client
{
  class AudioPlayer : public MediaPlayer
  {
  public:
    AudioPlayer();
    ~AudioPlayer() = default;
  };
}
