#pragma once

#include <string>
#include <cstddef>
#include <memory>

#include <idgen.hpp>
#include <client/per_process.hpp>
#include <common/media/types.hpp>
#include <common/events_v2/event_target.hpp>

namespace media_client
{
  using MediaEvent = events_comm::TrEvent<media_comm::TrMediaEventType>;

  /**
   * @class MediaPlayer
   * The `MediaPlayer` class represents a media player that can play audio or video content.
   */
  class MediaPlayer : public events_comm::TrEventTarget<media_comm::TrMediaEventType>
  {
    friend class ::TrClientContextPerProcess;

  public:
    /**
     * Constructs a `MediaPlayer` with the specified content type.
     *
     * @param contentType The type of media content (default: Audio).
     */
    explicit MediaPlayer(media_comm::MediaContentType contentType = media_comm::MediaContentType::Audio);
    virtual ~MediaPlayer() = default;

  public:
    /**
     * Checks if the media player can play the specified media type.
     *
     * @param mimeType A string specifying the MIME type of the media.
     * @return The result of the check.
     */
    CanPlayTypeResult canPlayType(const std::string &mimeType);

    /**
     * Captures the media stream.
     */
    void captureStream();

    /**
     * Seeks to the specified time in the media.
     *
     * @param time The time to seek to in milliseconds.
     */
    void fastSeek(long long time);

    /**
     * Loads the media source data (e.g., audio data) to be played.
     * `setSrc` must be called before this method to ensure the source data is set.
     */
    void load();

    /**
     * Pauses the media player.
     */
    void pause();

    /**
     * Plays the media player.
     */
    void play();

  public:
    /**
     * Sets the media source data (e.g., audio data) to be played.
     *
     * @param buffer The buffer containing the source data.
     * @param length The length of the source data.
     * @returns True if the source data is set successfully, false otherwise.
     */
    bool setSrc(const void *buffer, size_t length);

    /**
     * Gets the duration of the media in seconds.
     *
     * @returns The duration of the media in seconds.
     */
    double getDuration() const;

    /**
     * Gets if this media player is in loop mode.
     *
     * @returns True if the media player is in loop mode, false otherwise.
     */
    bool getLoop() const;

    /**
     * Sets if this media player is in loop mode.
     *
     * @param loop True to enable loop mode, false to disable it.
     */
    void setLoop(bool loop);

    /**
     * Gets the volume of this player.
     *
     * @returns The volume, a double value between 0 and 1.
     */
    double getVolume() const;

    /**
     * Sets the volume of this player.
     *
     * @param volume The volume to set, a double value between 0 and 1.
     * @throws std::invalid_argument if the volume is out of range.
     */
    void setVolume(double volume);

  private:
    uint32_t id;                                        // Unique identifier for the media player
    media_comm::MediaContentType contentType;           // The type of media content
    TrClientContextPerProcess *clientContext = nullptr; // The client context for making RPC calls
    const char *srcData = nullptr;                      // Pointer to the source data buffer
    size_t srcDataLength = 0;                           // Length of the source data buffer
    bool loopEnabled = false;                           // Whether loop mode is enabled
    double volume = 1.0;                                // The volume of the player, between 0 and 1
    double duration = -1.0;                             // The duration of the media in seconds

  private:
    inline static TrIdGenerator clientIdGen = TrIdGenerator(0x1f); // ID generator for media players
  };
} // namespace media_client
