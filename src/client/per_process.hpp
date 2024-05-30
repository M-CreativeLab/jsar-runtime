#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <node/node.h>
#include <node/node_api.h>
#include <napi.h>

#include "base.hpp"
#include "idgen.hpp"
#include "debug.hpp"
#include "ipc.hpp"
#include "common/messages.hpp"
#include "common/command_buffers/command_buffers.hpp"
#include "common/command_buffers/message.hpp"
#include "common/command_buffers/receiver.hpp"
#include "common/command_buffers/sender.hpp"
#include "common/events/message.hpp"
#include "common/events/receiver.hpp"
#include "common/events/sender.hpp"

using namespace std;
using namespace node;
using namespace v8;
using namespace commandbuffers;
using namespace events;

typedef uint32_t FrameRequestId;
typedef function<void(AnimationFrameRequest&)> FrameRequestCallback;

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
  string scriptSource;
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
  static TrClientContextPerProcess* Create();
  static TrClientContextPerProcess* Get();

public:
  TrClientContextPerProcess();
  ~TrClientContextPerProcess();

public:
  void start();
  void print();

public: // frame request methods
  FrameRequestId requestFrame(FrameRequestCallback callback);
  void cancelFrame(FrameRequestId id);

public: // event methods
  bool sendEventMessage(TrEventMessage &event);
  TrEventMessage* recvEventMessage(int timeout);

public: // command buffer methods
  bool sendCommandBufferRequest(TrCommandBufferBase &commandBuffer);
  TrCommandBufferResponse* recvCommandBufferResponse(int timeout);

private:
  void onListenFrames();

public:
  uint32_t id;
  string url;
  string applicationCacheDirectory;
  string httpsProxyServer;
  uint32_t webglVersion = 2;  // webgl2 by default
  uint32_t eventChanPort;
  uint32_t frameChanPort;
  uint32_t commandBufferChanPort;

private:
  ipc::TrOneShotClient<TrEventMessage> *eventChanClient = nullptr;
  TrEventSender *eventChanSender = nullptr;
  TrEventReceiver *eventChanReceiver = nullptr;
  ipc::TrOneShotClient<AnimationFrameRequest> *frameChanClient = nullptr;
  ipc::TrChannelReceiver<AnimationFrameRequest> *frameChanReceiver = nullptr;
  ipc::TrOneShotClient<TrCommandBufferMessage> *commandBufferChanClient = nullptr;
  TrCommandBufferSender *commandBufferChanSender = nullptr;
  TrCommandBufferReceiver *commandBufferChanReceiver = nullptr;

private:  // frame request fields
  map<FrameRequestId, FrameRequestCallback> frameRequestCallbacksMap;
  thread *framesListener = nullptr; // a thread to listen for frame requests
  mutex frameRequestMutex;
  atomic<bool> framesListenerRunning = false;

private:
  static TrClientContextPerProcess *s_Instance;
  static TrIdGenerator* s_IdGenerator;
};
