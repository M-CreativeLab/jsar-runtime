#pragma once

#include <string>
#include <memory>
#include <vector>
#include <shared_mutex>

#include <miniaudio/miniaudio.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "debug.hpp"
#include "common/classes.hpp"
#include "common/scoped_thread.hpp"
#include "common/ipc.hpp"
#include "common/media/message.hpp"
#include "common/media/sender.hpp"
#include "common/media/receiver.hpp"

using namespace std;
using namespace ipc;
using namespace media_comm;

class TrMediaManager;

/**
 * The sound source class which is representing the sound source in the audio engine, it provides the APIs for playing,
 * pausing, spatialization and other audio control operations.
 *
 * A sound source is managed by the audio engine only, and the other components should only reference the sound source
 * through the audio engine, such as each content or XR session would have possibly one or more sound sources, and they
 * should be managed by the audio engine.
 */
class TrSoundSource
{
public:
  TrSoundSource(TrMediaManager *mediaManager, std::shared_ptr<TrContentRuntime> content, uint32_t id);
  ~TrSoundSource();

public:
  /**
   * Play the sound source.
   */
  void play();
  /**
   * Pause the sound source.
   */
  void pause();
  /**
   * Seek the sound source to the specified position relative to the sound source.
   */
  void seek(float seconds);
  /**
   * Close the sound source, it will stop the sound source and release the resources.
   */
  void close();
  /**
   * Enable the spatialization for the sound source.
   */
  void enableSpatialization(bool enabled);
  /**
   * Set the audio data for the sound source.
   */
  void setSrcData(const char *audioBuffer, size_t sizeInBytes);
  /**
   * Set the volume for the sound source.
   */
  void setVolume(float volume);
  /**
   * Set if the sound source is playing in `looping` mode.
   */
  void setLooping(bool looping);
  /**
   * Set the base matrix for the sound source, the audio engine will use this matrix to calculate the sound source's
   * position and orientation.
   */
  void setBaseMatrix(glm::mat4 &baseMatrix);

private:
  bool dispatchMediaEvent(media_comm::TrMediaEventType eventType);
  bool dispatchMediaMetadata();
  void applyBaseMatrixToSound();
  void onBeforeData();
  void onAfterData();

public:
  uint32_t id;

private:
  TrMediaManager *mediaManager = nullptr;
  std::shared_ptr<TrContentRuntime> content = nullptr;
  std::unique_ptr<ma_sound> sound = nullptr;
  std::unique_ptr<ma_decoder> decoder = nullptr;
  glm::mat4 baseMatrix = glm::mat4(1.0f);
  bool autoPlay = true;
  bool isSrcDataLoaded = false;
  bool isEnded = true;

  friend class TrMediaManager;
};

/**
 * The media manager class which is for managing the audio and video's playbacks and recordings, and it also provides
 * the audio spatialization APIs for managing the audio sources and listeners.
 */
class TrMediaManager final
{
  friend class TrSoundSource;
  friend class TrContentRuntime;

public:
  static void DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

public:
  TrMediaManager(TrConstellation *constellation);
  ~TrMediaManager() = default;

public:
  /**
   * Initialize the media manager, it will start the audio engine.
   */
  void initialize();
  /**
   * Shutdown the media manager, it will stop the audio engine.
   */
  void shutdown();
  /**
   * Create a new sound source for the specified content.
   *
   * @param content The content which the sound source belongs to.
   * @param clientId The client id which the sound source is created for.
   * @returns The sound source if created successfully, otherwise returns `nullptr`.
   */
  shared_ptr<TrSoundSource> createSoundSource(std::shared_ptr<TrContentRuntime> content, uint32_t clientId);
  /**
   * Iterate all the sound sources by the specified content, and call the callback function for each sound source.
   *
   * @param content The content which the sound sources belong to.
   * @param callback The callback function which will be called for each sound source.
   */
  void iterateSoundSourcesByContent(std::shared_ptr<TrContentRuntime> content, std::function<void(shared_ptr<TrSoundSource>)> callback);
  /**
   * Find the sound source by the specified content and sound source's id.
   *
   * @param content The content which the sound source belongs to.
   * @param id The sound source's id.
   * @returns The sound source if found, otherwise returns `nullptr`.
   */
  shared_ptr<TrSoundSource> findSoundSource(std::shared_ptr<TrContentRuntime> content, uint32_t id);
  /**
   * Remove the sound sources by the specified content, it's used for releasing the sound sources when the content is
   * closed.
   *
   * @param contentId The content id which the sound sources belong to.
   */
  void removeSoundSourcesByContent(int contentId);
  /**
   * Update the listener's base matrix, the audio engine will use this matrix to calculate the listener's position and
   * orientation.
   *
   * @param baseMatrixValues The base matrix values in the column-major order.
   */
  void updateListenerBaseMatrix(float *baseMatrixValues)
  {
    glm::mat4 baseMatrix = glm::make_mat4(baseMatrixValues);
    updateListenerBaseMatrix(baseMatrix);
  }
  void updateListenerBaseMatrix(glm::mat4 &baseMatrix);

public:
  /**
   * It returns the command channel's port number for clients to connect.
   */
  inline int chanPort() { return commandChanServer->getPort(); }

private:
  void onNewChanClient(TrOneShotClient<TrMediaCommandMessage> &chanClient);
  void onContentRequest(std::shared_ptr<TrContentRuntime> content, TrMediaCommandMessage &reqMessage);
  void nextAudioData(void *pOutput, const void *pInput, ma_uint32 frameCount);

private:
  TrConstellation *constellation = nullptr;
  ma_engine audioEngine;
  ma_device audioDevice;
  shared_mutex mutexForSoundSources;
  vector<shared_ptr<TrSoundSource>> soundSources;
  unique_ptr<TrOneShotServer<TrMediaCommandMessage>> commandChanServer = nullptr;
  unique_ptr<WorkerThread> chanClientsWatcher = nullptr;
  atomic<bool> initialized = false;
  atomic<bool> disabled = false;
};
