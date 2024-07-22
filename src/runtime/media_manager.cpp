#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING
#endif
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#define MINIAUDIO_IMPLEMENTATION

#include "./media_manager.hpp"
#include <cassert>

void TrMediaManager::DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  TrMediaManager *mediaManager = (TrMediaManager *)pDevice->pUserData;
  if (mediaManager != nullptr)
    mediaManager->nextAudioData(pOutput, pInput, frameCount);
}

TrMediaManager::TrMediaManager()
{
}

void TrMediaManager::initialize()
{
  ma_device_config config = ma_device_config_init(ma_device_type_playback);
  config.playback.format = ma_format_f32;
  config.playback.channels = 2;
  config.sampleRate = 48000;
  config.dataCallback = TrMediaManager::DataCallback;
  config.pUserData = this;
  if (ma_device_init(NULL, &config, &audioDevice) != MA_SUCCESS)
    return;
  // ma_device_start(&audioDevice);
}

void TrMediaManager::shutdown()
{
  ma_device_uninit(&audioDevice);
}

void TrMediaManager::nextAudioData(void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  assert(pOutput != nullptr);
  assert(pInput == nullptr);
  assert(frameCount > 0);
  // TODO: Implement audio data processing
}
