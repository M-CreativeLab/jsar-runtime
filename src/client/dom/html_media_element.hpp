#pragma once

#include <string>
#include <memory>
#include "./html_element.hpp"
#include "../per_process.hpp"
#include "../media/media_player.hpp"

namespace dom
{
  enum class MediaReadyState
  {
    HAVE_NOTHING = 0,
    HAVE_METADATA = 1,
    HAVE_CURRENT_DATA = 2,
    HAVE_FUTURE_DATA = 3,
    HAVE_ENOUGH_DATA = 4
  };

  class HTMLMediaElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLMediaElement(std::string tagName,
                     media_comm::MediaContentType contentType,
                     weak_ptr<Document> ownerDocument)
        : HTMLElement(tagName, ownerDocument),
          clientContext(TrClientContextPerProcess::Get()),
          player_(clientContext->createMediaPlayer(contentType))
    {
      player_->resetGlobalEventListener([this](auto eventType, auto event)
                                        { onMediaEvent(eventType, event); });
    }
    virtual ~HTMLMediaElement()
    {
      player_->resetGlobalEventListener();
    }

  public:
    /**
     * Sets the media source URL.
     *
     * @param src The URL of the image to load.
     */
    void setSrc(const std::string &src)
    {
      setAttribute("src", src);
      loadMedia(src);
    }

    /**
     * Returns the media source URL.
     *
     * @returns The URL of the image to load.
     */
    std::string getSrc()
    {
      return getAttribute("src");
    }

    /**
     * Sets the media source buffer.
     *
     * @param data The buffer of the media source.
     * @param byteLength The length of the buffer.
     */
    inline void setSrcBuffer(const void *data, size_t byteLength)
    {
      onMediaLoaded(data, byteLength);
    }

    /**
     * Sets the volume of the media player.
     */
    inline void setVolume(bool value) { player_->setVolume(value); }

    /**
     * Returns the volume of the media player.
     */
    inline double getVolume() { return player_->getVolume(); }

    /**
     * Sets if this playback should be looped.
     */
    inline void setLoop(bool value) { player_->setLoop(value); }

    /**
     * Returns if this playback should be looped.
     */
    inline bool getLoop() { return player_->getLoop(); }

  public: // Read-only properties
    /**
     * Returns a string with the absolute URL of the chosen media resource.
     */
    std::string currentSrc() { return currentSrc_; }

    /**
     * A read-only double-precision floating-point value indicating the total duration of the media in seconds. If no media
     * data is available, the returned value is NaN. If the media is of indefinite length (such as streamed live media, a
     * WebRTC call's media, or similar), the value is +Infinity.
     */
    float duration() { return player_->getDuration(); }

    /**
     * Returns a boolean that indicates whether the media element has finished playing.
     */
    bool ended() { return ended_; }

  public: // Methods
    /**
     * It reports how likely it is that the current browser will be able to play media of a given MIME type.
     */
    inline media_comm::CanPlayTypeResult canPlayType(const std::string &mimeType)
    {
      return player_->canPlayType(mimeType);
    }

    /**
     * Quickly seeks the media to the new time with precision tradeoff.
     *
     * @param time The time to seek to.
     */
    inline void fastSeek(long long time) { player_->fastSeek(time); }

    /**
     * Resets the media element to its initial state and begins the process of selecting a media source and loading the
     * media in preparation for playback to begin at the beginning.
     */
    inline void load()
    {
      readyState = MediaReadyState::HAVE_NOTHING;
      player_->load();
    }

    /**
     * Pauses the media playback.
     */
    inline void pause()
    {
      if (readyState >= MediaReadyState::HAVE_CURRENT_DATA)
        player_->pause();
      else
        playScheduled_ = false;
    }

    /**
     * Starts the media playback.
     */
    inline void play()
    {
      if (readyState >= MediaReadyState::HAVE_CURRENT_DATA)
        player_->play();
      else
        playScheduled_ = true;
    }

    /**
     * Sets the media source object.
     *
     * @param callback The callback function that is called when the media event occurs.
     */
    inline void resetEventCallback(
        std::function<void(media_comm::TrMediaEventType, std::shared_ptr<media_client::MediaEvent>)> callback)
    {
      eventCallback_ = callback;
    }

  private:
    /**
     * Loads the image from the given URL.
     *
     * @param src The URL of the image to load.
     */
    void loadMedia(const std::string &src);

    /**
     * Callback function that is called when the image is loaded.
     */
    void onMediaLoaded(const void *mediaData, size_t mediaByteLength);

    /**
     * Callback function that is called when a media event occurs.
     */
    void onMediaEvent(media_comm::TrMediaEventType eventType,
                      std::shared_ptr<media_client::MediaEvent> event);

  public:
    /**
     * A boolean value that reflects the `autoplay` HTML attribute, indicating whether playback should automatically begin
     * as soon as enough media is available to do so without interruption.
     */
    bool autoPlay = false;
    /**
     * A boolean that reflects the `muted` HTML attribute, which indicates whether the media element's audio output should
     * be muted by default.
     */
    bool defaultMuted = false;
    /**
     * A `double` indicating the default playback rate for the media.
     */
    bool defaultPlaybackRate;
    /**
     * A double-precision floating-point value indicating the current playback time in seconds; if the media has not started
     * to play and has not been seeked, this value is the media's initial playback time. Setting this value seeks the media
     * to the new time. The time is specified relative to the media's timeline.
     */
    float currentTime;
    /**
     * A number which is one of the five possible state constants:
     *
     * - `HAVE_NOTHING` (0) - No information is available about the media resource.
     * - `HAVE_METADATA` (1) - Enough of the media resource has been retrieved that the metadata attributes are initialized.
     * - `HAVE_CURRENT_DATA` (2) - Data is available for the current playback position, but not enough to actually play more
     *  than one frame.
     * - `HAVE_FUTURE_DATA` (3) - Data for the current playback position as well as for at least a little bit of time into the
     * future is available (in this case, one frame's worth).
     * - `HAVE_ENOUGH_DATA` (4) - Enough data is available—and the download rate is high enough—that the media can be played
     * through to the end without interruption.
     */
    MediaReadyState readyState = MediaReadyState::HAVE_NOTHING;

  private:
    TrClientContextPerProcess *clientContext;
    std::shared_ptr<media_client::MediaPlayer> player_;
    std::string currentSrc_ = "";
    std::function<void(media_comm::TrMediaEventType, std::shared_ptr<media_client::MediaEvent>)> eventCallback_;
    float duration_ = 0;
    bool playScheduled_ = false;
    bool ended_ = false;
  };
}
