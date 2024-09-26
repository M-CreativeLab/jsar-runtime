#pragma once

#include <string>
#include "idgen.hpp"
#include "client/per_process.hpp"
#include "common/media/types.hpp"
#include "common/events_v2/event_target.hpp"

namespace media_client
{
  using MediaEvent = events_comm::TrEvent<media_comm::TrMediaEventType>;

  class MediaPlayer : public events_comm::TrEventTarget<media_comm::TrMediaEventType>
  {
  public:
    MediaPlayer(media_comm::MediaContentType contentType = media_comm::MediaContentType::Audio);
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
    /**
     * Load the media source data (e.g. audio data) to be played, `setSrc` must be called before this method to make sure
     * the source data is set.
     */
    void load();
    /**
     * Pause the media player.
     */
    void pause();
    /**
     * Play the media player.
     */
    void play();

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
     * Get the duration of the media in seconds.
     */
    double getDuration() const;
    /**
     * Get if this media player is in loop mode.
     */
    bool getLoop() const;
    /**
     * Set if this media player is in loop mode.
     */
    void setLoop(bool loop);
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
     */
    void setVolume(double volume);

  private:
    uint32_t id;
    MediaContentType contentType;
    TrClientContextPerProcess *clientContext = nullptr;
    const char *srcData = nullptr;
    size_t srcDataLength = 0;
    /**
     * If the loop mode is enabled.
     */
    bool loopEnabled = false;
    /**
     * The volume of this player, a double values must fall between 0 and 1, where 0 is effectively muted and 1 is the
     * loudest possible value.
     */
    double volume = 1.0;
    /**
     * The duration of this media, this value will be updated from the server side.
     */
    double duration = -1.0;

  private:
    static TrIdGenerator clientIdGen;

    friend class ::TrClientContextPerProcess;
  };
}
