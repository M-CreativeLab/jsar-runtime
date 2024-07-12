#include <iostream>
#include <cctype>
#include <vector>
#include <thread>

#include <unistd.h>
#include <sys/resource.h>

#include "per_process.hpp"
#include "crates/jsar_jsbindings.h"
#include "bindings.hpp"

using namespace std;
using namespace bindings;

#define TR_NAPI_MODULE_MAP(XX) \
  XX(canvas)                   \
  XX(env)                      \
  XX(messaging)                \
  XX(math3d)                   \
  XX(renderer)                 \
  XX(webgl)                    \
  XX(webxr)

#define XX(varname)                                                           \
  static napi_value __napi_reg_##varname(napi_env env, napi_value exports)    \
  {                                                                           \
    return Napi::RegisterModule(env, exports, bindings::varname::InitModule); \
  }                                                                           \
  static napi_module transmute_##varname##_napi_mod = {                       \
      NAPI_MODULE_VERSION,                                                    \
      node::ModuleFlags::kLinked,                                             \
      nullptr,                                                                \
      __napi_reg_##varname,                                                   \
      "transmute:" #varname,                                                  \
      nullptr,                                                                \
      {0},                                                                    \
  };
TR_NAPI_MODULE_MAP(XX)
#undef XX

ScriptEnvironment::ScriptEnvironment()
{
  const char *sourceData = reinterpret_cast<const char *>(get_jsbundle_ptr());
  size_t sourceSize = get_jsbundle_size();

  auto& args = scriptArgs;
  args.push_back("node");
  args.push_back("--experimental-vm-modules");
  args.push_back("--experimental-global-customevent");
  args.push_back("-e");
  args.push_back(string(sourceData, sourceSize));

  // TODO: Check if we are in debug mode
  static int debugPort = 9229;
  args.insert(args.begin() + 1, "--inspect=0.0.0.0:" + to_string(debugPort++));
}

ScriptEnvironment::~ScriptEnvironment()
{
}

bool ScriptEnvironment::initialize()
{
  string scriptArgsStr = "";
  for (auto &arg : scriptArgs)
  {
    if (arg.size() > 1024)
      scriptArgsStr += "<...>";
    else
      scriptArgsStr += arg + " ";
  }
  DEBUG(LOG_TAG_SCRIPT, "Executing Node.js script: %s", scriptArgsStr.c_str());
  std::unique_ptr<node::InitializationResult> result =
      node::InitializeOncePerProcess(scriptArgs, {node::ProcessInitializationFlags::kNoInitializeV8,
                                                  node::ProcessInitializationFlags::kNoInitializeNodeV8Platform,
                                                  node::ProcessInitializationFlags::kNoPrintHelpOrVersionOutput});

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
  std::unique_ptr<node::MultiIsolatePlatform> platform = node::MultiIsolatePlatform::Create(1);
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
  auto clientContext = TrClientContextPerProcess::Get();
  assert(clientContext != nullptr);
  if (clientContext->enableV8Profiling)
  {
    string logfile = clientContext->applicationCacheDirectory + "/v8.log"; // TODO: support multiple apps
    auto &envScriptArgs = scriptEnv.scriptArgs;
    envScriptArgs.insert(envScriptArgs.begin() + 1, "--prof");
    envScriptArgs.insert(envScriptArgs.begin() + 1, "--logfile=" + logfile);
    envScriptArgs.insert(envScriptArgs.begin() + 1, "--no_logfile_per_isolate");
  }
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

#define XX(name) AddLinkedBinding(nodeEnv, transmute_##name##_napi_mod);
    TR_NAPI_MODULE_MAP(XX)
#undef XX
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
  exit(exit_code);
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

TrClientContextPerProcess::TrClientContextPerProcess() : fontCacheManager(new font::FontCacheManager())
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

  // Clear for font cache
  if (fontCacheManager != nullptr)
  {
    delete fontCacheManager;
    fontCacheManager = nullptr;
  }

  // Clear for XR
  if (xrCommandChanSender != nullptr)
  {
    delete xrCommandChanSender;
    xrCommandChanSender = nullptr;
  }
  if (xrCommandChanReceiver != nullptr)
  {
    delete xrCommandChanReceiver;
    xrCommandChanReceiver = nullptr;
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
  eventChanClient = ipc::TrOneShotClient<TrEventMessage>::MakeAndConnect(eventChanPort, false);
  assert(eventChanClient != nullptr);
  frameChanClient = ipc::TrOneShotClient<TrFrameRequestMessage>::MakeAndConnect(frameChanPort, false);
  assert(frameChanClient != nullptr);
  commandBufferChanClient = ipc::TrOneShotClient<TrCommandBufferMessage>::MakeAndConnect(commandBufferChanPort, false);
  assert(commandBufferChanClient != nullptr);

  if (!eventChanClient->isConnected() || !frameChanClient->isConnected())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) failed to connect to the channels", id);
    return;
  }

  eventChanSender = new TrEventSender(eventChanClient);
  eventChanReceiver = new TrEventReceiver(eventChanClient);
  frameChanReceiver = new ipc::TrChannelReceiver<TrFrameRequestMessage>(frameChanClient);
  commandBufferChanSender = new TrCommandBufferSender(commandBufferChanClient);
  commandBufferChanReceiver = new TrCommandBufferReceiver(commandBufferChanClient);

  // XR device initialization
  if (xrDeviceInit.enabled && xrDeviceInit.commandChanPort > 0)
  {
    xrCommandChanClient = ipc::TrOneShotClient<xr::TrXRCommandMessage>::MakeAndConnect(xrDeviceInit.commandChanPort, false);
    xrCommandChanSender = new xr::TrXRCommandSender(xrCommandChanClient);
    xrCommandChanReceiver = new xr::TrXRCommandReceiver(xrCommandChanClient);
    xrInputSourcesZoneClient = std::make_unique<xr::TrXRInputSourcesZone>(xrDeviceInit.inputSourcesZonePath, TrZoneType::Client);
  }

  // Start the frames listener
  framesListenerRunning = true;
  framesListener = new thread([this]()
                              {
                                SET_THREAD_NAME("TrFramesListener");
                                this->onListenFrames(); });

  // Start the service alive listener
  serviceAliveListener = new thread([]()
                                    {
                                      SET_THREAD_NAME("TrServiceAliveListener");
                                      while (true)
                                      {
                                        this_thread::sleep_for(chrono::seconds(1));
                                        if (getppid() == 1)
                                          exit(0);  // FIXME: more graceful exit?
                                      } });

  startedAt = uv_hrtime();

  // Finish the client start.
  fprintf(stdout, "The client(%d) is started at %" PRIu64 ".\n", id, startedAt);
}

void TrClientContextPerProcess::print()
{
  fprintf(stdout, "ClientContext(%d) url=%s\n", id, url.c_str());
  fprintf(stdout, "ClientContext(%d) applicationCacheDirectory=%s\n", id, applicationCacheDirectory.c_str());
  fprintf(stdout, "ClientContext(%d) httpsProxyServer=%s\n", id, httpsProxyServer.c_str());
  fprintf(stdout, "ClientContext(%d) eventChanPort=%d\n", id, eventChanPort);
  fprintf(stdout, "ClientContext(%d) frameChanPort=%d\n", id, frameChanPort);
  fprintf(stdout, "ClientContext(%d) commandBufferChanPort=%d\n", id, commandBufferChanPort);

  if (xrDeviceInit.enabled == true)
  {
    fprintf(stdout, "ClientContext(%d) xrDeviceInit.active=%s\n", id, xrDeviceInit.active ? "YES" : "NO");
    fprintf(stdout, "ClientContext(%d) xrDeviceInit.stereoRenderingMode=%d\n", id,
            static_cast<int>(xrDeviceInit.stereoRenderingMode));
    fprintf(stdout, "ClientContext(%d) xrDeviceInit.commandChanPort=%d\n", id, xrDeviceInit.commandChanPort);
  }
  else
  {
    fprintf(stdout, "ClientContext(%d) xrDeviceInit.enabled=NO\n", id);
  }
}

void TrClientContextPerProcess::updateScriptTime()
{
  scriptAliveTime = uv_hrtime();
}

bool TrClientContextPerProcess::isScriptNotResponding(int timeoutDuration)
{
  auto actualDuration = static_cast<int>(uv_hrtime() - scriptAliveTime) / 1e6;
  return actualDuration > timeoutDuration;
}

FrameRequestId TrClientContextPerProcess::requestAnimationFrame(AnimationFrameRequestCallback callback)
{
  return requestFrame(TrFrameRequestType::AnimationFrame, [callback](frame_request::TrFrameRequestMessage &message)
                      {
                        auto animationFrameRequest = TrFrameRequestBase::MakeFromMessage<TrAnimationFrameRequest>(message);
                        if (animationFrameRequest == nullptr)
                          return;
                        callback(*animationFrameRequest);
                        delete animationFrameRequest; // End
                      });
}

FrameRequestId TrClientContextPerProcess::requestFrame(TrFrameRequestType type, TrFrameRequestFn fn)
{
  unique_lock<shared_mutex> lock(frameRequestMutex);
  FrameRequestId resId;
  while (true)
  {
    FrameRequestId id = static_cast<FrameRequestId>(s_IdGenerator->get());
    if (frameRequestCallbacksMap.find(id) != frameRequestCallbacksMap.end())
      continue;

    TrFrameRequestCallback callback(type, fn);
    frameRequestCallbacksMap.insert(pair<FrameRequestId, TrFrameRequestCallback>(id, callback));
    resId = id;
    break;
  }
  return resId;
}

void TrClientContextPerProcess::cancelFrame(FrameRequestId id)
{
  unique_lock<shared_mutex> lock(frameRequestMutex);
  frameRequestCallbacksMap.erase(id);
}

bool TrClientContextPerProcess::sendEventMessage(TrEventMessage &event)
{
  return eventChanSender->sendEvent(event);
}

TrEventMessage *TrClientContextPerProcess::recvEventMessage(int timeout)
{
  return eventChanReceiver->recvEvent(timeout);
}

bool TrClientContextPerProcess::sendCommandBufferRequest(TrCommandBufferBase &commandBuffer, bool followsFlush)
{
  if (isInXrFrame())
  {
    int viewIndex = 0;
    if (xrDeviceInit.renderedAsMultipass()) // If the device is rendered as multipass, the viewIndex should read from the frame request.
      viewIndex = currentXrFrameRequest->viewIndex;
    commandBuffer.renderingInfo = currentXrFrameRequest->createRenderingInfo(viewIndex);
  }
  bool success = commandBufferChanSender->sendCommandBufferRequest(commandBuffer, followsFlush);
  if (!isInXrFrame() || !followsFlush) // Directly returns success if not a XRFrame or not follow flush command buffer
    return success;
  else
    return success ? flushXrFrame() : false;
}

TrCommandBufferResponse *TrClientContextPerProcess::recvCommandBufferResponse(int timeout)
{
  return commandBufferChanReceiver->recvCommandBufferResponse(timeout);
}

bool TrClientContextPerProcess::startXrFrame(xr::TrXRFrameRequest *frameRequest)
{
  currentXrFrameRequest = frameRequest;
  XRFrameStartCommandBufferRequest req(frameRequest->stereoId, frameRequest->viewIndex);
  return sendCommandBufferRequest(req);
}

bool TrClientContextPerProcess::flushXrFrame()
{
  if (currentXrFrameRequest != nullptr)
  {
    auto stereoId = currentXrFrameRequest->stereoId;
    auto viewIndex = currentXrFrameRequest->viewIndex;
    XRFrameFlushCommandBufferRequest req(stereoId, viewIndex);
    return commandBufferChanSender->sendCommandBufferRequest(req, true);
  }
  else
  {
    return false;
  }
}

bool TrClientContextPerProcess::finishXrFrame(xr::TrXRFrameRequest *frameRequest)
{
  currentXrFrameRequest = nullptr;
  XRFrameEndCommandBufferRequest req(frameRequest->stereoId, frameRequest->viewIndex);
  return commandBufferChanSender->sendCommandBufferRequest(req, true);
}

xr::TrXRInputSourcesZone *TrClientContextPerProcess::getXRInputSourcesZone()
{
  return xrInputSourcesZoneClient.get();
}

int TrClientContextPerProcess::getFramebufferWidth() { return framebufferWidth; }
int TrClientContextPerProcess::getFramebufferHeight() { return framebufferHeight; }
void TrClientContextPerProcess::setFramebufferWidth(int w) { framebufferWidth = w; }
void TrClientContextPerProcess::setFramebufferHeight(int h) { framebufferHeight = h; }

font::FontCacheManager &TrClientContextPerProcess::getFontCacheManager()
{
  return *fontCacheManager;
}

void TrClientContextPerProcess::onListenFrames()
{
  while (framesListenerRunning)
  {
    TrFrameRequestMessage frameRequestMsg;
    if (!frameRequestMsg.deserialize(frameChanReceiver, -1))
      continue;

    auto msgType = frameRequestMsg.getType();
    if (TR_UNLIKELY(msgType == TrFrameRequestType::Unknown))
    {
      DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) received an unknown frame request message", id);
      continue;
    }

    // Check if the script is responsible.
    if (!isScriptNotResponding())
    {
      // Notify the frame request callbacks
      shared_lock<shared_mutex> lock(frameRequestMutex);
      for (auto &pair : frameRequestCallbacksMap)
      {
        auto callback = pair.second;
        if (callback.type == msgType)
          callback(frameRequestMsg);
      }
    }
  }
}
