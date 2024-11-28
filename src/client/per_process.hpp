#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <node/node.h>
#include <node/node_api.h>
#include <node/uv.h>
#include <napi.h>

#include "base.hpp"
#include "idgen.hpp"
#include "debug.hpp"
#include "common/ipc.hpp"
#include "common/zone.hpp"
#include "common/scoped_thread.hpp"
#include "common/analytics/perf_fs.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/command_buffers/command_buffers.hpp"
#include "common/command_buffers/receiver.hpp"
#include "common/command_buffers/sender.hpp"
#include "common/frame_request/types.hpp"
#include "common/events_v2/native_message.hpp"
#include "common/events_v2/native_receiver.hpp"
#include "common/events_v2/native_sender.hpp"
#include "common/font/cache.hpp"
#include "common/media/types.hpp"
#include "common/media/message.hpp"
#include "common/media/sender.hpp"
#include "common/media/receiver.hpp"
#include "common/xr/types.hpp"
#include "common/xr/input_sources.hpp"
#include "common/xr/message.hpp"
#include "common/xr/sender.hpp"
#include "common/xr/receiver.hpp"
#include "./classes.hpp"

using namespace std;
using namespace node;
using namespace v8;
using namespace ipc;
using namespace commandbuffers;
using namespace frame_request;
using namespace events_comm;
using namespace media_comm;

typedef uint32_t FrameRequestId;
typedef function<void(TrAnimationFrameRequest &)> AnimationFrameRequestCallback;

/**
 * `ScriptEnvironment` represents the environment for executing scripts within the application. It encapsulates the
 * necessary components and settings for initializing and running a Node.js-based script execution environment.

 * This class is responsible for:
 * - Managing the lifecycle of the script environment
 * - Initializing Node.js runtime
 * - Handling script arguments
 * - Providing access to the Node.js platform and initialization results

 * Each `ScriptEnvironment` instance corresponds to a separate script execution context, allowing for isolated and
 * controlled script execution within the application.
 */
class ScriptEnvironment
{
public:
  ScriptEnvironment(int id, string &scriptsDir);
  ~ScriptEnvironment();

public:
  bool initialize();
  bool initialized();
  void dispose();

public:
  int id;
  vector<string> scriptArgs;
  node::InitializationResult *nodeInitResult = nullptr;
  node::MultiIsolatePlatform *nodePlatform = nullptr;
};

class TrScriptRuntimePerProcess
{
private:
  static MaybeLocal<Value> PrepareStackTraceCallback(Local<Context> context,
                                                     Local<Value> exception,
                                                     Local<Array> trace);

public:
  TrScriptRuntimePerProcess();
  ~TrScriptRuntimePerProcess();

public:
  void start(vector<string> &scriptArgs);
  void terminate();

protected:
  int executeMainScript(ScriptEnvironment &env, vector<string> &scriptArgs);
  void onScriptExit(node::Environment *env, int exit_code);

private:
  bool started = false;
  bool running = false;
};

class TrClientPerformanceFileSystem : public analytics::PerformanceFileSystem
{
public:
  TrClientPerformanceFileSystem(std::string &cacheDir, const char *pidStr);
  ~TrClientPerformanceFileSystem() = default;

public:
  inline void setFps(int value) { fps->set(value); }
  inline void setFrameDuration(double value) { frameDuration->set(value); }
  inline void setLongFrames(int value) { longFrames->set(value); }

public:
  std::unique_ptr<analytics::PerformanceValue<int>> fps;
  std::unique_ptr<analytics::PerformanceValue<double>> frameDuration;
  std::unique_ptr<analytics::PerformanceValue<int>> longFrames;
};

/**
 * The client context is a singleton class in an application process.
 *
 * Every client process has a unique client context, which is responsible for managing the client-side resources, such as
 * the media players, command buffers, frame requests, and event channels.
 */
class TrClientContextPerProcess
{
public:
  /**
   * Create the client context instance, and throws an exception if the instance already exists.
   *
   * @returns The new instance of the client context.
   */
  static TrClientContextPerProcess *Create();
  /**
   * @returns The current instance of the client context.
   */
  static TrClientContextPerProcess *Get();

public:
  TrClientContextPerProcess();
  ~TrClientContextPerProcess();

public:
  /**
   * This function should be called at hive initialization to initialize the context-free client context.
   */
  void preload();
  /**
   * Initialize(start) the client context at specialized application process, such as connecting sockets, channels, etc.
   */
  void start();
  /**
   * Prints the client context information.
   */
  void print();

  /**
   * Get the current performance time in milliseconds.
   */
  inline double performanceNow() { return static_cast<double>(uv_hrtime() - startedAt) / 1e6; }

public: // SNR(Script Not Responsible) methods
  /**
   * Update the Script Alive Time to mark the script is responsible.
   */
  void updateScriptTime();
  /**
   * Check if the script is not responding.
   */
  inline bool isScriptNotResponding(int timeoutDuration = 2000);

public: // frame request methods
  FrameRequestId requestFrame(TrFrameRequestType type, TrFrameRequestFn callback);
  FrameRequestId requestAnimationFrame(AnimationFrameRequestCallback callback);
  void cancelFrame(FrameRequestId id);

public: // event methods
  /**
   * Send a native event to the host process.
   *
   * @param event The native event to send.
   * @returns true if the event is sent successfully.
   */
  bool sendEvent(TrNativeEvent &event);
  /**
   * Receive a native event message from the host process.
   *
   * @param timeout The timeout to wait for the next message.
   * @returns The new instance of the event message, or nullptr if no message received.
   */
  TrNativeEventMessage *recvEventMessage(int timeout);
  /**
   * Report a document event to the host process.
   */
  inline bool reportDocumentEvent(TrDocumentEventType documentEventType)
  {
    TrDocumentEvent detail(id, documentEventType);
    auto event = TrNativeEvent::MakeEvent(TrNativeEventType::DocumentEvent, &detail);
    return sendEvent(event);
  }
  /**
   * Send an RPC request to the host process and waits for the response.
   *
   * @param method The method name to call.
   * @param args The arguments to pass to the method.
   * @returns true if the RPC call is successful and the response is received.
   */
  inline bool makeRpcCall(string method, vector<string> args)
  {
    TrRpcRequest req(id, method, args);
    auto event = TrNativeEvent::MakeEvent(TrNativeEventType::RpcRequest, &req);
    if (sendEvent(event))
    {
      // TODO: wait for the response, moved from `NativeEventTarget`.
    }
    return false;
  }

public: // media methods
  /**
   * Create a new media player, it returns a shared pointer to the created player.
   */
  shared_ptr<media_client::MediaPlayer> createMediaPlayer(media_comm::MediaContentType contentType = media_comm::MediaContentType::Audio);
  /**
   * Create a new audio player, it returns a shared pointer to the created player.
   */
  shared_ptr<media_client::AudioPlayer> createAudioPlayer();
  /**
   * Send a media command to the media channel.
   */
  bool sendMediaRequest(TrMediaCommandBase &mediaCommand)
  {
    assert(mediaChanSender != nullptr);
    return mediaChanSender->sendCommand(mediaCommand);
  }

public: // commandbuffer methods
  bool sendCommandBufferRequest(TrCommandBufferBase &commandBuffer, bool followsFlush = false);
  TrCommandBufferResponse *recvCommandBufferResponse(int timeout);

public: // WebXR methods
  bool startXrFrame(xr::TrXRFrameRequest *frameRequest);
  bool flushXrFrame();
  bool finishXrFrame(xr::TrXRFrameRequest *frameRequest);
  inline bool isInXrFrame() { return currentXrFrameRequest != nullptr; }
  inline xr::TrXRFrameRequest *getCurrentXRFrameRequest() { return currentXrFrameRequest; }
  xr::TrXRDeviceContextZone *getXRDeviceContextZone() { return xrDeviceContextZoneClient.get(); }
  xr::TrXRInputSourcesZone *getXRInputSourcesZone() { return xrInputSourcesZoneClient.get(); }

  /**
   * Get the framebuffer's width, or zero if the XR is not enabled.
   *
   * @returns the framebuffer's width
   */
  int getFramebufferWidth() { return xrDeviceContextZoneClient == nullptr ? 0 : xrDeviceContextZoneClient->getFramebufferConfig().width; }
  /**
   * Get the framebuffer's height, or zero if the XR is not enabled.
   *
   * @returns the framebuffer's height
   */
  int getFramebufferHeight() { return xrDeviceContextZoneClient == nullptr ? 0 : xrDeviceContextZoneClient->getFramebufferConfig().height; }
  /**
   * Check if the framebuffer is double-wide, or false if the XR is not enabled.
   *
   * Double-wide is used at desktop example app, which doesn't depend on any other OpenGL extension like OVR_multiview, but at the production device, the framebuffer
   * is commonly not double-wide.
   *
   * @returns true if the framebuffer is double width
   */
  bool isFramebufferDoubleWide() { return xrDeviceContextZoneClient != nullptr && xrDeviceContextZoneClient->getFramebufferConfig().useDoubleWide; }

  template <typename CommandType>
  bool sendXrCommand(xr::TrXRCommandBase<CommandType> &xrCommand)
  {
    if (!xrCommandChanSender)
      return false;
    if (currentXrFrameRequest != nullptr)
    {
      xrCommand.stereoId = currentXrFrameRequest->stereoId;
      xrCommand.stereoTimestamp = currentXrFrameRequest->stereoTimestamp;
      auto now = chrono::time_point_cast<chrono::milliseconds>(chrono::steady_clock::now());
      xrCommand.sentAtTimestamp = now.time_since_epoch().count();
    }
    return xrCommandChanSender->sendCommand(xrCommand);
  }

  template <typename CommandType>
  CommandType *recvXrCommand(xr::TrXRCmdType type, int timeout)
  {
    if (!xrCommandChanReceiver)
      return nullptr;
    auto message = xrCommandChanReceiver->recvCommandMessage(timeout);
    if (message == nullptr)
      return nullptr;
    if (message->type != type) // When the message is not the expected type, discard it.
    {
      delete message;
      return nullptr;
    }
    auto xrCommand = message->createInstance<CommandType>();
    delete message;
    return xrCommand;
  }

public:
  font::FontCacheManager &getFontCacheManager() { return *fontCacheManager; }
  TrClientPerformanceFileSystem &getPerfFs() { return *perfFs; }

private:
  void onListenMediaEvent(media_comm::TrMediaCommandMessage &eventMessage);

public:
  uint32_t id;
  string url;
  /**
   * The directory where the application can store files that are persistent.
   */
  string applicationCacheDirectory;
  /**
   * The https proxy server to use for network requests if proxy is enabled.
   */
  string httpsProxyServer;
  /**
   * Enable v8 profiling.
   */
  bool enableV8Profiling = false;
  uint32_t webglVersion = 2; // webgl2 by default
  uint32_t eventChanPort;
  uint32_t frameChanPort;
  uint32_t mediaChanPort;
  uint32_t commandBufferChanPort;
  xr::TrDeviceInit xrDeviceInit;
  uint64_t startedAt;

private: // event fields
  TrOneShotClient<events_comm::TrNativeEventMessage> *eventChanClient = nullptr;
  events_comm::TrNativeEventSender *eventChanSender = nullptr;
  events_comm::TrNativeEventReceiver *eventChanReceiver = nullptr;

private: // frame fields
  TrOneShotClient<TrFrameRequestMessage> *frameChanClient = nullptr;
  TrChannelReceiver<TrFrameRequestMessage> *frameChanReceiver = nullptr;

private: // media fields
  TrOneShotClient<TrMediaCommandMessage> *mediaChanClient = nullptr;
  unique_ptr<TrMediaCommandSender> mediaChanSender = nullptr;
  unique_ptr<TrMediaCommandReceiver> mediaChanReceiver = nullptr;
  unique_ptr<WorkerThread> mediaEventsPollingWorker = nullptr;
  vector<shared_ptr<media_client::MediaPlayer>> mediaPlayers;

private: // command buffer fields
  TrOneShotClient<TrCommandBufferMessage> *commandBufferChanClient = nullptr;
  TrCommandBufferSender *commandBufferChanSender = nullptr;
  TrCommandBufferReceiver *commandBufferChanReceiver = nullptr;

private: // xr fields
  TrOneShotClient<xr::TrXRCommandMessage> *xrCommandChanClient = nullptr;
  xr::TrXRCommandSender *xrCommandChanSender = nullptr;
  xr::TrXRCommandReceiver *xrCommandChanReceiver = nullptr;
  xr::TrXRFrameRequest *currentXrFrameRequest = nullptr;
  unique_ptr<xr::TrXRDeviceContextZone> xrDeviceContextZoneClient;
  unique_ptr<xr::TrXRInputSourcesZone> xrInputSourcesZoneClient;
  int framebufferWidth = 0;
  int framebufferHeight = 0;

private: // frame request fields
  map<FrameRequestId, TrFrameRequestCallback> frameRequestCallbacksMap;
  shared_mutex frameRequestMutex;

private: // service & script alive checking fields
  thread *serviceAliveListener = nullptr;
  atomic<uint64_t> scriptAliveTime = 0;

private: // other fields
  unique_ptr<font::FontCacheManager> fontCacheManager = nullptr;
  unique_ptr<TrClientPerformanceFileSystem> perfFs = nullptr;

private:
  static TrClientContextPerProcess *s_Instance;
  static TrIdGenerator *s_IdGenerator;
};
