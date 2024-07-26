#pragma once

#include <string>
#include "idgen.hpp"
#include "client/per_process.hpp"

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

  public:
    bool setSrc(void* buffer, size_t length);

  private:
    uint32_t id;
    TrClientContextPerProcess* clientContext = nullptr;
    const char* srcData = nullptr;
    size_t srcDataLength = 0;

  private:
    static TrIdGenerator clientIdGen;
  };
}
