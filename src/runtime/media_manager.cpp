#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING
#endif
#define MINIAUDIO_IMPLEMENTATION

#include "./media_manager.hpp"
#include "runtime/content.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

TrSoundSource::TrSoundSource(TrMediaManager *mediaManager,
                             TrContentRuntime *content,
                             uint32_t id)
    : mediaManager(mediaManager), content(content), id(id)
{
}

TrSoundSource::~TrSoundSource()
{
  ma_sound_uninit(sound.get());
  ma_decoder_uninit(decoder.get());
}

void TrSoundSource::play()
{
  if (isSrcDataLoaded)
    ma_sound_start(sound.get());
}

void TrSoundSource::pause()
{
  if (isSrcDataLoaded)
    ma_sound_stop(sound.get());
}

void TrSoundSource::seek(float seconds)
{
  // TODO: implement this function
}

void TrSoundSource::close()
{
  if (isSrcDataLoaded)
    ma_sound_stop(sound.get());
}

void TrSoundSource::enableSpatialization(bool enabled)
{
  ma_sound_set_spatialization_enabled(sound.get(), enabled ? MA_TRUE : MA_FALSE);
}

void TrSoundSource::setSrcData(const char *audioBuffer, size_t sizeInBytes)
{
  if (sound == nullptr)
    sound = make_unique<ma_sound>();
  if (decoder == nullptr)
    decoder = make_unique<ma_decoder>();

  dispatchMediaEvent(media_comm::TrMediaEventType::LoadStart);
  ma_sound *pSound = sound.get();
  ma_decoder *pDecoder = decoder.get();
  ma_decoder_config decoderConfig = ma_decoder_config_init(TR_MEDIA_OUTPUT_FORMAT,
                                                           TR_MEDIA_OUTPUT_CHANNELS,
                                                           TR_MEDIA_OUTPUT_SAMPLE_RATE);
  ma_decoder_init_memory(audioBuffer, sizeInBytes, &decoderConfig, pDecoder);
  dispatchMediaMetadata();
  dispatchMediaEvent(media_comm::TrMediaEventType::LoadedMetadata);
  dispatchMediaEvent(media_comm::TrMediaEventType::LoadedData);

  ma_sound_config soundConfig = ma_sound_config_init();
  soundConfig.pFilePath = nullptr;
  soundConfig.pDataSource = pDecoder;
  soundConfig.channelsOut = 0;
  soundConfig.isLooping = MA_FALSE;
  soundConfig.flags = MA_SOUND_FLAG_DECODE;
  ma_sound_init_ex(&mediaManager->audioEngine, &soundConfig, pSound);
  ma_sound_set_spatialization_enabled(pSound, MA_TRUE);
  ma_sound_set_attenuation_model(pSound, ma_attenuation_model_exponential);
  applyBaseMatrixToSound();
  isSrcDataLoaded = true;
}

void TrSoundSource::setVolume(float volume)
{
  ma_sound_set_volume(sound.get(), volume);
}

void TrSoundSource::setLooping(bool looping)
{
  ma_sound_set_looping(sound.get(), looping ? MA_TRUE : MA_FALSE);
}

void TrSoundSource::setBaseMatrix(glm::mat4 &baseMatrix)
{
  this->baseMatrix = baseMatrix;
  applyBaseMatrixToSound();
}

bool TrSoundSource::dispatchMediaEvent(media_comm::TrMediaEventType eventType)
{
  media_comm::TrOnMediaEvent event(eventType, id);
  return content->dispatchMediaEvent(event);
}

bool TrSoundSource::dispatchMediaMetadata()
{
  media_comm::TrOnMediaMetadata metadata(id);
  {
    // Update the media metadata
    ma_uint64 framesCount;
    ma_decoder_get_length_in_pcm_frames(decoder.get(), &framesCount);
    metadata.duration = static_cast<double>(framesCount) / decoder->outputSampleRate;
  }
  return content->dispatchMediaEvent(metadata);
}

void TrSoundSource::applyBaseMatrixToSound()
{
  if (sound == nullptr)
    return;
  ma_sound *pSound = sound.get();
  auto position = baseMatrix[3];
  auto forward = glm::normalize(glm::vec3(-glm::column(baseMatrix, 2)));
  ma_sound_set_position(pSound, position.x, position.y, position.z);
  ma_sound_set_direction(pSound, forward.x, forward.y, forward.z);
}

void TrSoundSource::onBeforeData()
{
  if (sound != nullptr && isSrcDataLoaded)
  {
    ma_sound *pSound = sound.get();
    auto soundAtEnd = ma_sound_get_at_end(pSound);
    if (!isEnded && soundAtEnd)
      dispatchMediaEvent(media_comm::TrMediaEventType::Ended);
    isEnded = soundAtEnd;
  }
}

void TrSoundSource::onAfterData()
{
  if (sound != nullptr && isSrcDataLoaded)
  {
    // TODO
  }
}

void TrMediaManager::DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  TrMediaManager *mediaManager = (TrMediaManager *)pDevice->pUserData;
  if (mediaManager != nullptr)
    mediaManager->nextAudioData(pOutput, pInput, frameCount);
}

TrMediaManager::TrMediaManager(TrConstellation *constellation)
    : constellation(constellation), initialized(false), soundSources()
{
  {
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = TR_MEDIA_OUTPUT_FORMAT;
    deviceConfig.playback.channels = TR_MEDIA_OUTPUT_CHANNELS;
    deviceConfig.sampleRate = TR_MEDIA_OUTPUT_SAMPLE_RATE;
    deviceConfig.dataCallback = TrMediaManager::DataCallback;
    deviceConfig.pUserData = this;
    if (ma_device_init(NULL, &deviceConfig, &audioDevice) != MA_SUCCESS)
      DEBUG(LOG_TAG_ERROR, "Failed to initialize audio device");
  }
  {
    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.pDevice = &audioDevice;
    engineConfig.noAutoStart = MA_TRUE;
    engineConfig.listenerCount = 1;
    if (ma_engine_init(&engineConfig, &audioEngine) != MA_SUCCESS)
      DEBUG(LOG_TAG_ERROR, "Failed to initialize audio engine");

    ma_engine_listener_set_enabled(&audioEngine, 0, MA_TRUE);
    ma_engine_listener_set_cone(&audioEngine, 0, 60.0f, 90.0f, 0.3f);
  }
}

void TrMediaManager::initialize()
{
  ma_engine_start(&audioEngine);
  commandChanServer = make_unique<TrOneShotServer<TrMediaCommandMessage>>("mediaCommandChan");
  chanClientsWatcher = make_unique<WorkerThread>("mediaClientsWatcher", [this](WorkerThread &_worker)
                                                 { commandChanServer->tryAccept([this](TrOneShotClient<TrMediaCommandMessage> &chanClient)
                                                                                { onNewChanClient(chanClient); },
                                                                                1000); });
  initialized = true;
}

void TrMediaManager::shutdown()
{
  if (initialized)
  {
    chanClientsWatcher->stop();
    ma_engine_stop(&audioEngine);
  }
  DEBUG(LOG_TAG_MEDIA, "TrMediaManager::shutdown() done.");
}

shared_ptr<TrSoundSource> TrMediaManager::createSoundSource(TrContentRuntime *content, uint32_t clientId)
{
  if (!initialized)
    return nullptr;

  unique_lock<shared_mutex> lock(mutexForSoundSources);
  auto newSound = make_shared<TrSoundSource>(this, content, clientId);
  soundSources.push_back(newSound);
  return newSound;
}

void TrMediaManager::iterateSoundSourcesByContent(TrContentRuntime *content, std::function<void(shared_ptr<TrSoundSource>)> callback)
{
  if (!content)
    return;

  shared_lock<shared_mutex> lock(mutexForSoundSources);
  for (auto &soundSource : soundSources)
  {
    if (soundSource->content == content)
      callback(soundSource);
  }
}

shared_ptr<TrSoundSource> TrMediaManager::findSoundSourceById(uint32_t id)
{
  shared_lock<shared_mutex> lock(mutexForSoundSources);
  for (auto &soundSource : soundSources)
  {
    if (soundSource->id == id)
      return soundSource;
  }
  return nullptr;
}

void TrMediaManager::updateListenerBaseMatrix(glm::mat4 &baseMatrix)
{
  if (!initialized)
    return;
  auto position = baseMatrix[3];
  auto forward = glm::normalize(glm::vec3(-glm::column(baseMatrix, 2)));
  auto up = glm::normalize(glm::vec3(glm::column(baseMatrix, 1)));
  ma_engine_listener_set_position(&audioEngine, 0, position.x, position.y, position.z);
  ma_engine_listener_set_direction(&audioEngine, 0, forward.x, forward.y, forward.z);
  ma_engine_listener_set_world_up(&audioEngine, 0, up.x, up.y, up.z);
}

void TrMediaManager::onNewChanClient(TrOneShotClient<TrMediaCommandMessage> &chanClient)
{
  auto content = constellation->contentManager->getContent(chanClient.getCustomId());
  if (content == nullptr)
    commandChanServer->removeClient(&chanClient);
  else
    content->onMediaChanConnected(chanClient);
}

void TrMediaManager::onContentRequest(TrContentRuntime *content, TrMediaCommandMessage &reqMessage)
{
  auto messageType = reqMessage.getType();
  if (messageType == TrMediaCommandType::CreateSoundRequest)
  {
    auto createSoundReq = TrMediaCommandBase::CreateFromMessage<TrCreateSoundRequest>(reqMessage);
    createSoundSource(content, createSoundReq.clientId);
  }
  else if (messageType == TrMediaCommandType::PlayRequest)
  {
    auto playReq = TrMediaCommandBase::CreateFromMessage<TrPlayRequest>(reqMessage);
    auto soundSource = findSoundSourceById(playReq.clientId);
    if (soundSource != nullptr && soundSource->content == content)
      soundSource->play();
  }
  else if (messageType == TrMediaCommandType::PauseRequest)
  {
    auto pauseReq = TrMediaCommandBase::CreateFromMessage<TrPauseRequest>(reqMessage);
    auto soundSource = findSoundSourceById(pauseReq.clientId);
    if (soundSource != nullptr && soundSource->content == content)
      soundSource->pause();
  }
  else if (messageType == TrMediaCommandType::CloseRequest)
  {
    auto closeReq = TrMediaCommandBase::CreateFromMessage<TrCloseRequest>(reqMessage);
    auto soundSource = findSoundSourceById(closeReq.clientId);
    if (soundSource != nullptr && soundSource->content == content)
      soundSource->close();
  }
  else if (messageType == TrMediaCommandType::SetSrcDataRequest)
  {
    auto setSrcDataReq = TrMediaCommandBase::CreateFromMessage<TrSetSrcDataRequest>(reqMessage);
    auto soundSource = findSoundSourceById(setSrcDataReq.clientId);
    if (soundSource != nullptr && soundSource->content == content)
    {
      soundSource->setSrcData((char *)setSrcDataReq.srcData, setSrcDataReq.sizeInBytes);
      soundSource->setVolume(setSrcDataReq.initialVolume);
      soundSource->setLooping(setSrcDataReq.loopingAtStart);
    }
  }
  else if (messageType == TrMediaCommandType::SetVolumeRequest)
  {
    auto setVolumeReq = TrMediaCommandBase::CreateFromMessage<TrSetVolumeRequest>(reqMessage);
    auto soundSource = findSoundSourceById(setVolumeReq.clientId);
    if (soundSource != nullptr && soundSource->content == content)
      soundSource->setVolume(setVolumeReq.volume);
  }
  else if (messageType == TrMediaCommandType::SetLoopingRequest)
  {
    auto setLoopingReq = TrMediaCommandBase::CreateFromMessage<TrSetLoopingRequest>(reqMessage);
    auto soundSource = findSoundSourceById(setLoopingReq.clientId);
    if (soundSource != nullptr && soundSource->content == content)
      soundSource->setLooping(setLoopingReq.looping);
  }
  else if (messageType == TrMediaCommandType::EnableAudioSpatializationRequest)
  {
    auto enableSpatializationReq = TrMediaCommandBase::CreateFromMessage<TrEnableAudioSpatializationRequest>(reqMessage);
    auto soundSource = findSoundSourceById(enableSpatializationReq.clientId);
    if (soundSource != nullptr && soundSource->content == content)
      soundSource->enableSpatialization(enableSpatializationReq.enabled);
  }
  else
  {
    DEBUG(LOG_TAG_ERROR, "Unknown media command type: %d", messageType);
  }
}

void TrMediaManager::nextAudioData(void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  for (auto &soundSource : soundSources)
    soundSource->onBeforeData();
  ma_engine_read_pcm_frames(&audioEngine, pOutput, frameCount, nullptr);
  for (auto &soundSource : soundSources)
    soundSource->onAfterData();
}
