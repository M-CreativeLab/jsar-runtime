#pragma once

#include <atomic>
#include <thread>

#include <node/node.h>
#include <node/node_api.h>
#include <napi.h>

#include "base.hpp"
#include "bindings.hpp"
#include "bindings/env/env.hpp"

using namespace std;
using namespace bindings;

NODE_API_LINKED_MODULE(env, "transmute:env", InitEnvModule);
NODE_API_LINKED_MODULE(logger, "transmute:logger", InitLoggerModule);
NODE_API_LINKED_MODULE(messaging, "transmute:messaging", InitMessagingModule);
NODE_API_LINKED_MODULE(renderer, "transmute:renderer", InitRendererModule);
NODE_API_LINKED_MODULE(webgl, "transmute:webgl", InitWebglModule);
NODE_API_LINKED_MODULE(webxr, "transmute:webxr", InitWebxrModule);

class NodeBootstrapper
{
private:
  static NodeBootstrapper *instance_;

public:
  static NodeBootstrapper *GetOrCreateInstance()
  {
    if (instance_ == nullptr)
      instance_ = new NodeBootstrapper();
    return instance_;
  }
  static bool IsInstanceRunning()
  {
    return instance_ != nullptr && instance_->m_running;
  }

public:
  NodeBootstrapper();
  ~NodeBootstrapper();

  bool initialize();
  bool start();
  void waitForCompletion();
  bool isRunning();
  void disposeV8();
  TransmuteEnvironment *getEnv();
  bool isRuntimeAvailable();

private:
  int runNodeInstance();

private:
  std::thread *m_thread;
  atomic<bool> m_started = false;
  atomic<bool> m_running = false;
  // config
  atomic<bool> m_disposeV8OnExit = false;

  node::InitializationResult *m_nodeInitRes = nullptr;
  node::MultiIsolatePlatform *m_nodePlatform = nullptr;
};
