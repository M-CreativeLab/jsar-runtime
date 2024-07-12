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
#include "ipc.hpp"
#include "common/zone.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/command_buffers/command_buffers.hpp"
#include "common/command_buffers/receiver.hpp"
#include "common/command_buffers/sender.hpp"
#include "common/frame_request/types.hpp"
#include "common/events/message.hpp"
#include "common/events/receiver.hpp"
#include "common/events/sender.hpp"
#include "common/font/cache.hpp"
#include "common/xr/types.hpp"
#include "common/xr/input_sources.hpp"
#include "common/xr/message.hpp"
#include "common/xr/sender.hpp"
#include "common/xr/receiver.hpp"

using namespace std;
using namespace node;
using namespace v8;
using namespace commandbuffers;
using namespace frame_request;
using namespace events;

typedef uint32_t FrameRequestId;
typedef function<void(TrAnimationFrameRequest &)> AnimationFrameRequestCallback;

class ScriptEnvironment
{
public:
  ScriptEnvironment();
  ~ScriptEnvironment();

public:
  bool initialize();
  bool initialized();
  void dispose();

public:
  vector<string> scriptArgs;
  node::InitializationResult *nodeInitResult = nullptr;
  node::MultiIsolatePlatform *nodePlatform = nullptr;
};

class TrScriptRuntimePerProcess
{
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

class TrClientContextPerProcess
{
public:
  static TrClientContextPerProcess *Create();
  static TrClientContextPerProcess *Get();

public:
  TrClientContextPerProcess();
  ~TrClientContextPerProcess();

public:
  void start();
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
  bool sendEventMessage(TrEventMessage &event);
  TrEventMessage *recvEventMessage(int timeout);

public: // command buffer methods
  bool sendCommandBufferRequest(TrCommandBufferBase &commandBuffer, bool followsFlush = false);
  TrCommandBufferResponse *recvCommandBufferResponse(int timeout);

public: // WebXR methods
  bool startXrFrame(xr::TrXRFrameRequest *frameRequest);
  bool flushXrFrame();
  bool finishXrFrame(xr::TrXRFrameRequest *frameRequest);
  inline bool isInXrFrame() { return currentXrFrameRequest != nullptr; }
  xr::TrXRInputSourcesZone *getXRInputSourcesZone();

  int getFramebufferWidth();
  int getFramebufferHeight();
  void setFramebufferWidth(int w);
  void setFramebufferHeight(int h);

  template <typename CommandType>
  bool sendXrCommand(xr::TrXRCommandBase<CommandType> &xrCommand)
  {
    if (!xrCommandChanSender)
      return false;
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

public: // font cache methods
  font::FontCacheManager &getFontCacheManager();

private:
  void onListenFrames();

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
  uint32_t commandBufferChanPort;
  xr::TrDeviceInit xrDeviceInit;
  uint64_t startedAt;

private:
  ipc::TrOneShotClient<TrEventMessage> *eventChanClient = nullptr;
  TrEventSender *eventChanSender = nullptr;
  TrEventReceiver *eventChanReceiver = nullptr;
  ipc::TrOneShotClient<TrFrameRequestMessage> *frameChanClient = nullptr;
  ipc::TrChannelReceiver<TrFrameRequestMessage> *frameChanReceiver = nullptr;
  ipc::TrOneShotClient<TrCommandBufferMessage> *commandBufferChanClient = nullptr;
  TrCommandBufferSender *commandBufferChanSender = nullptr;
  TrCommandBufferReceiver *commandBufferChanReceiver = nullptr;
  font::FontCacheManager *fontCacheManager = nullptr;

private: // xr fields
  ipc::TrOneShotClient<xr::TrXRCommandMessage> *xrCommandChanClient = nullptr;
  xr::TrXRCommandSender *xrCommandChanSender = nullptr;
  xr::TrXRCommandReceiver *xrCommandChanReceiver = nullptr;
  xr::TrXRFrameRequest *currentXrFrameRequest = nullptr;
  std::unique_ptr<xr::TrXRInputSourcesZone> xrInputSourcesZoneClient;
  int framebufferWidth = 0;
  int framebufferHeight = 0;

private: // frame request fields
  map<FrameRequestId, TrFrameRequestCallback> frameRequestCallbacksMap;
  thread *framesListener = nullptr; // a thread to listen for frame requests
  shared_mutex frameRequestMutex;
  atomic<bool> framesListenerRunning = false;

private: // service & script alive checking fields
  thread *serviceAliveListener = nullptr;
  atomic<uint64_t> scriptAliveTime = 0;

private:
  static TrClientContextPerProcess *s_Instance;
  static TrIdGenerator *s_IdGenerator;
};
