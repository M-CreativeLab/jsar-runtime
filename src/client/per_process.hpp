#pragma once

#include <string>
#include <vector>
#include <node/node.h>
#include <node/node_api.h>
#include <napi.h>

#include "base.hpp"
#include "debug.hpp"
#include "ipc.hpp"

using namespace std;
using namespace node;
using namespace v8;

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

public:
  uint32_t id;
  string url;
  string applicationCacheDirectory;
  string httpsProxyServer;
  uint32_t channelServerPort;

private:
  ipc::TrChannelSender<ipc::CustomEvent> *channelSender;

private:
  static TrClientContextPerProcess *s_Instance;
};
