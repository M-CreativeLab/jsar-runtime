#pragma once

#include <string>

namespace media
{
  class MediaPlayer
  {
  public:
    MediaPlayer();
    virtual ~MediaPlayer() = default;

  public:
    bool canPlayType(const std::string &type);
    void captureStream();
    void fastSeek(long long time);
    void load();
    void pause();
    void play();
    void setMediaKeys();
    void setSinkId(std::string &sinkId);
  };
}
