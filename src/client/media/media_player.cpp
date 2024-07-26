#include "./media_player.hpp"
#include "common/media/message.hpp"

namespace media
{
  TrIdGenerator MediaPlayer::clientIdGen = TrIdGenerator(0x1f);
  MediaPlayer::MediaPlayer() : clientContext(TrClientContextPerProcess::Get())
  {
    assert(clientContext != nullptr);
    id = clientIdGen.get();
    media_comm::TrCreateSoundRequest request(id);
    clientContext->sendMediaRequest(request);
  }

  bool MediaPlayer::canPlayType(const std::string &type)
  {
    // Method implementation
    return false;
  }

  void MediaPlayer::captureStream()
  {
    // Method implementation
  }

  void MediaPlayer::fastSeek(long long time)
  {
    // Method implementation
  }

  void MediaPlayer::load()
  {
    if (srcData == nullptr || srcDataLength == 0)
      return;
    assert(clientContext != nullptr);
    media_comm::TrSetSrcDataRequest request(id, (void *)srcData, srcDataLength);
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

  void MediaPlayer::setMediaKeys()
  {
    // Method implementation
  }

  void MediaPlayer::setSinkId(std::string &sinkId)
  {
    // Method implementation
  }

  bool MediaPlayer::setSrc(void *buffer, size_t length)
  {
    if (buffer == nullptr || length == 0)
      return false;

    srcData = (char *)malloc(length);
    if (srcData == nullptr)
      return false;

    memcpy((void *)srcData, buffer, length);
    srcDataLength = length;
    return true;
  }
}
