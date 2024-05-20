#include <thread>
#include "per_process.hpp"
#include "crates/jsar_jsbindings.h"
#include "bindings.hpp"

using namespace bindings;

#define NODE_API_LINKED_MODULE(varname, modname, regfunc)              \
  static napi_value __napi_##regfunc(napi_env env, napi_value exports) \
  {                                                                    \
    return Napi::RegisterModule(env, exports, regfunc);                \
  }                                                                    \
  static napi_module transmute_##varname##_napi_mod = {                \
      NAPI_MODULE_VERSION,                                             \
      node::ModuleFlags::kLinked,                                      \
      nullptr,                                                         \
      __napi_##regfunc,                                                \
      modname,                                                         \
      nullptr,                                                         \
      {0},                                                             \
  };

NODE_API_LINKED_MODULE(canvas, "transmute:canvas", InitCanvasModule);
NODE_API_LINKED_MODULE(env, "transmute:env", InitEnvModule);
NODE_API_LINKED_MODULE(logger, "transmute:logger", InitLoggerModule);
NODE_API_LINKED_MODULE(messaging, "transmute:messaging", InitMessagingModule);
NODE_API_LINKED_MODULE(renderer, "transmute:renderer", InitRendererModule);
NODE_API_LINKED_MODULE(webgl, "transmute:webgl", InitWebglModule);
// NODE_API_LINKED_MODULE(webxr, "transmute:webxr", InitWebxrModule);
#undef NODE_API_LINKED_MODULE

ScriptEnvironment::ScriptEnvironment()
{
  const char *sourceData = reinterpret_cast<const char *>(get_jsbundle_ptr());
  size_t sourceSize = get_jsbundle_size();
  scriptSource = string(sourceData, sourceSize);

  vector<string> args = {
      "node",
      "--experimental-vm-modules",
      "--experimental-global-customevent",
      "-e",
      scriptSource};

  // TODO: Check if we are in debug mode
  args.insert(args.begin() + 1, "--inspect");
  scriptArgs = args;
}

ScriptEnvironment::~ScriptEnvironment()
{
}

bool ScriptEnvironment::initialize()
{
  DEBUG(LOG_TAG_SCRIPT, "initializing script with Node.js");
  std::unique_ptr<node::InitializationResult> result =
      node::InitializeOncePerProcess(scriptArgs, {node::ProcessInitializationFlags::kNoInitializeV8,
                                                  node::ProcessInitializationFlags::kNoInitializeNodeV8Platform,
                                                  node::ProcessInitializationFlags::kNoPrintHelpOrVersionOutput,
                                                  node::ProcessInitializationFlags::kNoDefaultSignalHandling});

  auto errors = result->errors();
  if (errors.size() > 0)
  {
    DEBUG(LOG_TAG_SCRIPT, "Failed to initialize ScriptEnvironment, there are errors:");
    for (uint32_t n = 0; n < errors.size(); n++)
      DEBUG(LOG_TAG_SCRIPT, "[%d]: %s", n, errors[n].c_str());
  }

  if (result->early_return() != 0)
  {
    DEBUG(LOG_TAG_SCRIPT, "Early return: %d", result->early_return());
    return result->exit_code() != 0;
  }
  else
  {
    nodeInitResult = result.release();
  }

  // Initialize the v8/nodejs platform.
  std::unique_ptr<node::MultiIsolatePlatform> platform = node::MultiIsolatePlatform::Create(4);
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();
  nodePlatform = platform.release();
  return true;
}

bool ScriptEnvironment::initialized()
{
  return nodeInitResult != nullptr && nodePlatform != nullptr;
}

void ScriptEnvironment::dispose()
{
  V8::Dispose();
  V8::DisposePlatform();
  node::TearDownOncePerProcess();
}

TrScriptRuntimePerProcess::TrScriptRuntimePerProcess()
{
}

TrScriptRuntimePerProcess::~TrScriptRuntimePerProcess()
{
}

void TrScriptRuntimePerProcess::start(vector<string> &scriptArgs)
{
  if (running == true)
  {
    DEBUG(LOG_TAG_SCRIPT, "ScriptRuntime is already running.");
    return;
  }
  if (started == true)
  {
    DEBUG(LOG_TAG_SCRIPT, "ScriptRuntime is already started.");
    return;
  }

  auto scriptEnv = ScriptEnvironment();
  scriptEnv.initialize();

  executeMainScript(scriptEnv, scriptArgs);
  scriptEnv.dispose();
}

void TrScriptRuntimePerProcess::terminate()
{
  running = false;
}

int TrScriptRuntimePerProcess::executeMainScript(ScriptEnvironment &env, vector<string> &scriptArgs)
{
  SET_THREAD_NAME("TrScript");

  int exit_code = 0;
  auto nodePlatform = env.nodePlatform;
  auto nodeInitResult = env.nodeInitResult;

  // Combine the script arguments with the default arguments.
  vector<string> args(nodeInitResult->args());
  for (uint32_t n = 0; n < scriptArgs.size(); n++)
    args.push_back(scriptArgs[n]);

  // Setup up a libuv event loop, v8::Isolate, and Node.js Environment.
  vector<string> errors;
  auto setup = node::CommonEnvironmentSetup::Create(nodePlatform,
                                                    &errors,
                                                    args,
                                                    nodeInitResult->exec_args());

  if (!setup)
  {
    DEBUG(LOG_TAG_SCRIPT, "Failed to setup the script environment.");
    for (uint n = 0; n < errors.size(); n++)
      DEBUG(LOG_TAG_SCRIPT, "[%d]: %s", n, errors[n].c_str());
    return false;
  }

  v8::Isolate *isolate = setup->isolate();
  node::Environment *nodeEnv = setup->env();
  node::SetProcessExitHandler(nodeEnv, [this](node::Environment *env, int exit_code)
                              { this->onScriptExit(env, exit_code); });

  {
    Locker locker(isolate);
    Isolate::Scope isolateScope(isolate);
    HandleScope handleScope(isolate);
    Context::Scope contextScope(setup->context());

    // Add the transmute:core module to the Node.js instance.
    AddLinkedBinding(nodeEnv, transmute_env_napi_mod);
    AddLinkedBinding(nodeEnv, transmute_logger_napi_mod);
    AddLinkedBinding(nodeEnv, transmute_messaging_napi_mod);
    AddLinkedBinding(nodeEnv, transmute_renderer_napi_mod);
    // AddLinkedBinding(env, transmute_webaudio_napi_mod);
    AddLinkedBinding(nodeEnv, transmute_canvas_napi_mod);
    AddLinkedBinding(nodeEnv, transmute_webgl_napi_mod);
    // AddLinkedBinding(nodeEnv, transmute_webxr_napi_mod);
    // The followings are created by Rust
    // AddLinkedBinding(nodeEnv, transmute_htmlrender_napi_mod);

    MaybeLocal<Value> ret = node::LoadEnvironment(nodeEnv, node::StartExecutionCallback{});
    if (ret.IsEmpty())
      return 1;

    exit_code = node::SpinEventLoop(nodeEnv).FromMaybe(1);
    node::Stop(nodeEnv);
    SET_THREAD_NAME("TrScript_Stopped");

    {
      /**
       * Removing the globals after Node.js instance is stopped.
       */

      // Dispose the default audio context
      // disposeDefaultAudioContext();
    }
  }
  return exit_code;
}

void TrScriptRuntimePerProcess::onScriptExit(node::Environment *env, int exit_code)
{
  DEBUG(LOG_TAG_SCRIPT, "Script exited with code %d", exit_code);
}

TrClientContextPerProcess *TrClientContextPerProcess::s_Instance = nullptr;
TrIdGenerator *TrClientContextPerProcess::s_IdGenerator = new TrIdGenerator(1);

TrClientContextPerProcess *TrClientContextPerProcess::Create()
{
  if (s_Instance == nullptr)
    s_Instance = new TrClientContextPerProcess();
  return s_Instance;
}
TrClientContextPerProcess *TrClientContextPerProcess::Get()
{
  return s_Instance;
}

TrClientContextPerProcess::TrClientContextPerProcess()
{
}

TrClientContextPerProcess::~TrClientContextPerProcess()
{
  if (eventChanSender != nullptr)
  {
    delete eventChanSender;
    eventChanSender = nullptr;
  }
  if (eventChanReceiver != nullptr)
  {
    delete eventChanReceiver;
    eventChanReceiver = nullptr;
  }
  if (frameChanReceiver != nullptr)
  {
    delete frameChanReceiver;
    frameChanReceiver = nullptr;
  }
  // Clear for frame request callbacks
  frameRequestCallbacksMap.clear();
  framesListenerRunning = false;
  if (framesListener != nullptr)
  {
    framesListener->join();
    delete framesListener;
    framesListener = nullptr;
  }
}

void TrClientContextPerProcess::start()
{
  eventChanClient = ipc::TrOneShotClient<CustomEvent>::MakeAndConnect(eventChanPort, false);
  frameChanClient = ipc::TrOneShotClient<AnimationFrameRequest>::MakeAndConnect(frameChanPort, false);

  if (!eventChanClient->isConnected() || !frameChanClient->isConnected())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) failed to connect to the channels", id);
    return;
  }

  eventChanSender = new ipc::TrChannelSender<CustomEvent>(eventChanClient);
  eventChanReceiver = new ipc::TrChannelReceiver<CustomEvent>(eventChanClient);
  frameChanReceiver = new ipc::TrChannelReceiver<AnimationFrameRequest>(frameChanClient);

  // Start the frames listener
  framesListenerRunning = true;
  framesListener = new thread([this]()
                              { 
                                SET_THREAD_NAME("TrFramesListener");
                                this->onListenFrames(); });
}

void TrClientContextPerProcess::print()
{
  DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) url=%s", id, url.c_str());
  DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) applicationCacheDirectory=%s", id, applicationCacheDirectory.c_str());
  DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) httpsProxyServer=%s", id, httpsProxyServer.c_str());
  DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) eventChanPort=%d", id, eventChanPort);
  DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) frameChanPort=%d", id, frameChanPort);
}

FrameRequestId TrClientContextPerProcess::requestFrame(FrameRequestCallback callback)
{
  lock_guard<mutex> lock(frameRequestMutex);
  FrameRequestId resId;
  while (true)
  {
    FrameRequestId id = static_cast<FrameRequestId>(s_IdGenerator->get());
    if (frameRequestCallbacksMap.find(id) != frameRequestCallbacksMap.end())
      continue;

    frameRequestCallbacksMap.insert(pair<FrameRequestId, FrameRequestCallback>(id, callback));
    resId = id;
    break;
  }
  return resId;
}

void TrClientContextPerProcess::cancelFrame(FrameRequestId id)
{
  lock_guard<mutex> lock(frameRequestMutex);
  frameRequestCallbacksMap.erase(id);
}

void TrClientContextPerProcess::onListenFrames()
{
  while (framesListenerRunning)
  {
    auto frameRequest = frameChanReceiver->tryRecv(-1);
    if (frameRequest == nullptr)
      continue;

    {
      // Notify the frame request callbacks
      lock_guard<mutex> lock(frameRequestMutex);
      for (auto &pair : frameRequestCallbacksMap)
      {
        auto callback = pair.second;
        callback(*frameRequest);
      }
    }
  }
}
