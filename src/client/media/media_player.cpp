#include "./media_player.hpp"
#include "common/media/message.hpp"

namespace media_client
{
  MediaPlayer::MediaPlayer(MediaContentType contentType)
      : events_comm::TrEventTarget<media_comm::TrMediaEventType>()
      , clientContext(TrClientContextPerProcess::Get())
      , contentType(contentType)
  {
    assert(clientContext != nullptr);
    id = clientIdGen.get();
    media_comm::TrCreateSoundRequest request(id);
    clientContext->sendMediaRequest(request);
  }

  CanPlayTypeResult MediaPlayer::canPlayType(const std::string &mimeType)
  {
    if (
      mimeType == "audio/mpeg" ||
      mimeType == "audio/wav")
      return CanPlayTypeResult::Probably;
    else
      return CanPlayTypeResult::No;
  }

  void MediaPlayer::captureStream()
  {
    // Method implementation
  }

  void MediaPlayer::fastSeek(long long time)
  {
    if (srcData == nullptr || srcDataLength == 0)
      return;
    assert(clientContext != nullptr);
    media_comm::TrSeekRequest request(id, time);
    clientContext->sendMediaRequest(request);
  }

  void MediaPlayer::load()
  {
    if (srcData == nullptr || srcDataLength == 0)
      return;
    assert(clientContext != nullptr);
    media_comm::TrSetSrcDataRequest request(id, (void *)srcData, srcDataLength);
    request.initialVolume = volume;
    request.loopingAtStart = loopEnabled;
    clientContext->sendMediaRequest(request);
  }

  void MediaPlayer::pause()
  {
    assert(clientContext != nullptr);
    media_comm::TrPauseRequest request(id);
    clientContext->sendMediaRequest(request);
  }

  void MediaPlayer::play()
  {
    assert(clientContext != nullptr);
    media_comm::TrPlayRequest request(id);
    clientContext->sendMediaRequest(request);
  }

  bool MediaPlayer::setSrc(const void *buffer, size_t length)
  {
    if (buffer == nullptr || length == 0)
      return false;

    if (srcData != nullptr)
    {
      free((void *)srcData);
      srcData = nullptr;
      srcDataLength = 0;
    }

    srcData = (char *)malloc(length);
    if (srcData == nullptr)
      return false;

    memcpy((void *)srcData, buffer, length);
    srcDataLength = length;
    return true;
  }

  double MediaPlayer::getDuration() const
  {
    return duration;
  }

  bool MediaPlayer::getLoop() const
  {
    return loopEnabled;
  }

  void MediaPlayer::setLoop(bool loop)
  {
    loopEnabled = loop;
    if (clientContext != nullptr)
    {
      media_comm::TrSetLoopingRequest request(id, loopEnabled);
      clientContext->sendMediaRequest(request);
    }
  }

  double MediaPlayer::getVolume() const
  {
    return volume;
  }

  void MediaPlayer::setVolume(double volume)
  {
    this->volume = volume;
    if (clientContext != nullptr)
    {
      media_comm::TrSetVolumeRequest request(id, volume);
      clientContext->sendMediaRequest(request);
    }
  }
}
