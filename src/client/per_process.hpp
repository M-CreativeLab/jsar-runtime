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

using namespace std;
using namespace node;
using namespace v8;

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

private:
  void onListenFrames();

public:
  uint32_t id;
  string url;
  string applicationCacheDirectory;
  string httpsProxyServer;
  uint32_t eventChanPort;
  uint32_t frameChanPort;

private:
  ipc::TrOneShotClient<CustomEvent> *eventChanClient = nullptr;
  ipc::TrChannelSender<CustomEvent> *eventChanSender = nullptr;
  ipc::TrChannelReceiver<CustomEvent> *eventChanReceiver = nullptr;
  ipc::TrOneShotClient<AnimationFrameRequest> *frameChanClient = nullptr;
  ipc::TrChannelReceiver<AnimationFrameRequest> *frameChanReceiver = nullptr;

private:  // frame request fields
  map<FrameRequestId, FrameRequestCallback> frameRequestCallbacksMap;
  thread *framesListener = nullptr; // a thread to listen for frame requests
  mutex frameRequestMutex;
  atomic<bool> framesListenerRunning = false;

private:
  static TrClientContextPerProcess *s_Instance;
  static TrIdGenerator* s_IdGenerator;
};
