#pragma once

#include <functional>
#include "audio_context.hpp"

namespace webaudio
{
  void *adgCreate(float sampleRate, std::function<void(int numberOfFrames, void *outputBuffer, void *inputBuffer)> renderFn);
  void adgDestroy(void *handle);
  bool adgStart(void *handle);
  bool adgStop(void *handle);
  bool adgStartRecording(void *handle);
  bool adgStopRecording(void *handle);
}
