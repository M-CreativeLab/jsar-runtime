#pragma once

#include <string>
#include "idgen.hpp"
#include "client/per_process.hpp"

namespace media
{
  enum class CanPlayTypeResult
  {
    No = 0,
    Probably = 1,
    Maybe = 2
  };

  class MediaPlayer
  {
  public:
    MediaPlayer();
    virtual ~MediaPlayer() = default;

  public:
    /**
     * Check if the media player can play the specified media type.
     *
     * @param mimeType A string specifying the MIME type of the media.
     * @return The result of the check.
     */
    CanPlayTypeResult canPlayType(const std::string &mimeType);
    void captureStream();
    void fastSeek(long long time);
    void load();
    void pause();
    void play();
    void setMediaKeys();
    void setSinkId(std::string &sinkId);

  public:
    /**
     * Set the media source data (e.g. audio data) to be played.
     * 
     * @param buffer The buffer containing the source data.
     * @param length The length of the source data.
     * @returns True if the source data is set successfully, false otherwise.
     */
    bool setSrc(void *buffer, size_t length);
    /**
     * Get the volume of this player.
     * 
     * @returns The volume, a double value between 0 and 1.
     */
    double getVolume() const;
    /**
     * Set the volume of this player.
     *
     * @param volume The volume to set, a double values must fall between 0 and 1, where 0 is effectively muted and 1 is the
     * loudest possible value.
     * @return True if the volume is set successfully, false otherwise.
     */
    bool setVolume(double volume);

  private:
    uint32_t id;
    TrClientContextPerProcess *clientContext = nullptr;
    const char *srcData = nullptr;
    size_t srcDataLength = 0;
    /**
     * The volume of this player, a double values must fall between 0 and 1, where 0 is effectively muted and 1 is the
     * loudest possible value.
     */
    double volume = 0;

  private:
    static TrIdGenerator clientIdGen;
  };
}
