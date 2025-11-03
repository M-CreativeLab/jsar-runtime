#include "./audio_player.hpp"

namespace endor
{
  namespace media_client
  {
    AudioPlayer::AudioPlayer()
        : MediaPlayer(MediaContentType::Audio)
    {
    }
  }
} // namespace endor