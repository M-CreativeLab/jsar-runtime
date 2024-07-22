#pragma once

#include <miniaudio/miniaudio.h>
#include <string>

class TrMediaManager
{
public:
  static void DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

public:
  TrMediaManager();
  ~TrMediaManager() = default;

public:
  void initialize();
  void shutdown();

private:
  void nextAudioData(void *pOutput, const void *pInput, ma_uint32 frameCount);

private:
  ma_device audioDevice;
};
