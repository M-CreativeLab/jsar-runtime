#include <iostream>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <thread>

#include <unistd.h>
#include <sys/resource.h>
#include <crates/bindings.hpp>

#include "./per_process.hpp"
#include "./browser/window.hpp"
#include "./builtin_scene/scene.hpp"
#include "./dom/dom_scripting.hpp"
#include "./graphics/webgl_context.hpp"
#include "./media/media_player.hpp"
#include "./media/audio_player.hpp"
#include "./xr/device.hpp"
#include "./bindings.hpp"

using namespace std;
using namespace node;
using namespace v8;
using namespace bindings;

#define XX(varname)                                                           \
  static napi_value __napi_reg_##varname(napi_env env, napi_value exports)    \
  {                                                                           \
    return Napi::RegisterModule(env, exports, bindings::varname::InitModule); \
  }                                                                           \
  static napi_module transmute_##varname##_napi_mod = {                       \
    NAPI_MODULE_VERSION,                                                      \
    node::ModuleFlags::kLinked,                                               \
    nullptr,                                                                  \
    __napi_reg_##varname,                                                     \
    "transmute:" #varname,                                                    \
    nullptr,                                                                  \
    {0},                                                                      \
  };
TR_NAPI_MODULE_MAP(XX)
#undef XX

ScriptEnvironment::ScriptEnvironment(int id, string &scriptsDir)
    : id(id)
{
  auto &args = scriptArgs;
  args.push_back("node");
  args.push_back("--experimental-vm-modules");
  args.push_back("--experimental-global-customevent");
  /**
   * Add --preserve-symlinks and --preserve-symlinks-main to avoid the call to `realpathSync()` when resolving modules and optimize its performance.
   */
  {
    args.push_back("--preserve-symlinks");
    args.push_back("--preserve-symlinks-main");
  }
  args.push_back("-r");
  args.push_back(scriptsDir + "/jsar-bootstrap-babylon.js");
  args.push_back(scriptsDir + "/jsar-client-entry.js");

  // TODO: Check if we are in debug mode
  int inspectPort = 9229 + id - 1;
  args.insert(args.begin() + 1, "--inspect=0.0.0.0:" + to_string(inspectPort));

  fprintf(stdout, "To debug with Chrome Inspector, it's recommend to use in your host computer:\n");
  fprintf(stdout, "  adb forward tcp:9229 tcp:%d\n", inspectPort);
}

ScriptEnvironment::~ScriptEnvironment()
{
}

void printNodeEnv(const string &name)
{
  char *str = getenv(name.c_str());
  if (str != nullptr)
    std::cout << "  " << name << ": " << str << std::endl;
  else
    std::cout << "  " << name << ": (null)" << std::endl;
}

bool ScriptEnvironment::initialize()
{
  string scriptArgsStr = "";
  for (auto &arg : scriptArgs)
    scriptArgsStr += arg + " ";

  std::cout << "Node.js commandline: " << scriptArgsStr << std::endl;
  {
    std::cout << "Environment Variables List:" << std::endl;
    printNodeEnv("NODE_DEBUG");
    printNodeEnv("NODE_OPTIONS");
  }

  std::unique_ptr<node::InitializationResult> result =
    node::InitializeOncePerProcess(scriptArgs,
                                   {node::ProcessInitializationFlags::kNoInitializeV8,
                                    node::ProcessInitializationFlags::kNoInitializeNodeV8Platform,
                                    node::ProcessInitializationFlags::kNoDefaultSignalHandling});

  auto errors = result->errors();
  if (errors.size() > 0)
  {
    std::cerr << "Failed to initialize ScriptEnvironment, there are errors:" << std::endl;
    for (uint32_t n = 0; n < errors.size(); n++)
      std::cerr << "[" << n << "]: " << errors[n] << std::endl;
  }

  if (result->early_return())
  {
    std::cerr << "Failed to initialize ScriptEnvironment, early return." << std::endl;
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

MaybeLocal<Value> TrScriptRuntimePerProcess::PrepareStackTraceCallback(Local<Context> context,
                                                                       Local<Value> exception,
                                                                       Local<Array> trace)
{
  if (::dom::ContextEmbedderTag::IsMyContext(context))
  {
    /**
     * Prepare the stack trace for our own context.
     */
    Isolate *isolate = context->GetIsolate();
    HandleScope handleScope(isolate);
    Local<String> errorWithStack = exception->ToString(context).ToLocalChecked();
    Local<String> breakLine = String::NewFromUtf8(isolate, "\n").ToLocalChecked();
    Local<String> beforeLine = String::NewFromUtf8(isolate, "    at ").ToLocalChecked();

    for (uint32_t n = 0; n < trace->Length(); n++)
    {
      Local<Value> stackFrame = trace->Get(context, n).ToLocalChecked();
      Local<String> stackFrameString;
      if (stackFrame->ToString(context).ToLocal(&stackFrameString))
      {
        errorWithStack = String::Concat(isolate, errorWithStack, breakLine);
        errorWithStack = String::Concat(isolate, errorWithStack, beforeLine);
        errorWithStack = String::Concat(isolate, errorWithStack, stackFrameString);
      }
    }
    return errorWithStack;
  }
  else
  {
    return node::PrepareStackTraceCallback(context, exception, trace);
  }
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

  auto clientContext = TrClientContextPerProcess::Get();
  assert(clientContext != nullptr);

  string scriptsDir = clientContext->applicationCacheDirectory + "/scripts";
  ScriptEnvironment &scriptEnv = clientContext->createScriptingEnv(clientContext->id, scriptsDir);
  if (clientContext->enableV8Profiling)
  {
    string logfile = clientContext->applicationCacheDirectory + "/v8.log"; // TODO: support multiple apps
    auto &envScriptArgs = scriptEnv.scriptArgs;
    envScriptArgs.insert(envScriptArgs.begin() + 1, "--prof");
    envScriptArgs.insert(envScriptArgs.begin() + 1, "--logfile=" + logfile);
    envScriptArgs.insert(envScriptArgs.begin() + 1, "--no_logfile_per_isolate");
  }
  scriptEnv.initialize();

  clientContext->reportDocumentEvent(TrDocumentEventType::BeforeScripting);
  executeMainScript(scriptEnv, scriptArgs);
  scriptEnv.dispose();
}

void TrScriptRuntimePerProcess::terminate()
{
  running = false;
}

int TrScriptRuntimePerProcess::executeMainScript(ScriptEnvironment &env, vector<string> &scriptArgs)
{
  int exit_code = 0;
  auto nodePlatform = env.nodePlatform;
  auto nodeInitResult = env.nodeInitResult;

  // Combine the script arguments with the default arguments.
  vector<string> args(nodeInitResult->args());
  for (uint32_t n = 0; n < scriptArgs.size(); n++)
    args.push_back(scriptArgs[n]);

  auto &execArgs = nodeInitResult->exec_args();
  {
    // Print the command line arguments and the exec arguments.
    std::cout << "Command Args: ";
    for (auto &arg : args)
      std::cout << "(" << arg << ") ";
    std::cout << std::endl;

    std::cout << "   Exec Args: ";
    for (auto &arg : execArgs)
      std::cout << "(" << arg << ") ";
    std::cout << std::endl;
  }

  // Setup up a libuv event loop, v8::Isolate, and Node.js Environment.
  vector<string> errors;
  auto setup = node::CommonEnvironmentSetup::Create(nodePlatform,
                                                    &errors,
                                                    args,
                                                    execArgs);

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
  isolate->SetPrepareStackTraceCallback(PrepareStackTraceCallback);

  {
    Locker locker(isolate);
    Isolate::Scope isolateScope(isolate);
    HandleScope handleScope(isolate);
    Context::Scope contextScope(setup->context());

#define XX(name) AddLinkedBinding(nodeEnv, transmute_##name##_napi_mod);
    TR_NAPI_MODULE_MAP(XX)
#undef XX

    MaybeLocal<Value> ret = node::LoadEnvironment(nodeEnv, node::StartExecutionCallback{});
    if (ret.IsEmpty())
      return 1;

    exit_code = node::SpinEventLoop(nodeEnv).FromMaybe(1);
    node::Stop(nodeEnv);
  }
  DEBUG(LOG_TAG_SCRIPT, "Script execution finished with code: %d", exit_code);
  return exit_code;
}

void TrScriptRuntimePerProcess::onScriptExit(node::Environment *env, int exit_code)
{
  DEBUG(LOG_TAG_ERROR, "Script exited with code: %d", exit_code);
  exit(2);
}

TrClientPerformanceFileSystem::TrClientPerformanceFileSystem(std::string &cacheDir, const char *pidStr)
    : analytics::PerformanceFileSystem(cacheDir, pidStr)
{
  assert(pidStr != nullptr);
  fps = makeValue<int>("fps", 0);
  frameDuration = makeValue<double>("frame_duration", 0.0);
  longFrames = makeValue<int>("long_frames", 0);
}

static bool GetBooleanEnv(const char *name, bool default_value = false)
{
  char *str = std::getenv(name);
  size_t len = str ? strlen(str) : 0;

  if (str == nullptr || len == 0)
    return default_value;

  if (len == 1)
  {
    if (*str == '0')
      return false;
    else if (isspace(*str))
      return default_value; // Treat whitespace as default value.
    else
      return true;
  }
  else if (len == 2)
  {
    if (str[0] == 'n' && str[1] == 'o')
      return false;
    else
      return default_value; // Treat other two-character strings as default value.
  }
  else if (len == 3)
  {
    if (str[0] == 'y' && str[1] == 'e' && str[2] == 's')
      return true;
    else
      return default_value; // Treat other three-character strings as default value.
  }
  else
  {
    // For longer strings, check for specific values.
    if (strcmp(str, "true") == 0)
      return true;
    if (strcmp(str, "false") == 0)
      return false;
  }

  // Default to false if the value is not recognized.
  return default_value;
}

TrClientEnvironmentPerProcess::TrClientEnvironmentPerProcess()
    : debugLayoutTree(GetBooleanEnv("DEBUG_LAYOUT_TREE"))
    , debugLayoutFormattingContext(GetBooleanEnv("DEBUG_LAYOUT_FORMATTING_CONTEXT"))
{
}

TrClientContextPerProcess *TrClientContextPerProcess::s_Instance = nullptr;
TrIdGenerator *TrClientContextPerProcess::s_IdGenerator = new TrIdGenerator(1);

TrClientContextPerProcess *TrClientContextPerProcess::Create()
{
  if (s_Instance != nullptr)
    throw std::runtime_error("TrClientContextPerProcess is already created.");
  s_Instance = new TrClientContextPerProcess();
  return s_Instance;
}

TrClientContextPerProcess *TrClientContextPerProcess::Get()
{
  assert(s_Instance != nullptr && "Fatel Error: TrClientContextPerProcess is not created.");
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
}

void TrClientContextPerProcess::preload()
{
  fontCacheManager = std::make_unique<font::FontCacheManager>();
}

void TrClientContextPerProcess::start()
{
  string pid = to_string(getpid());
  perfFs = std::make_unique<TrClientPerformanceFileSystem>(applicationCacheDirectory, pid.c_str());

  // Required channels
  eventChanClient = ipc::TrOneShotClient<TrNativeEventMessage>::MakeAndConnect(eventChanPort, false, id);
  assert(eventChanClient != nullptr);
  mediaChanClient = ipc::TrOneShotClient<TrMediaCommandMessage>::MakeAndConnect(mediaChanPort, false, id);
  assert(mediaChanClient != nullptr);
  commandBufferChanClient = ipc::TrOneShotClient<TrCommandBufferMessage>::MakeAndConnect(commandBufferChanPort, false, id);
  assert(commandBufferChanClient != nullptr);

  if (
    !eventChanClient->isConnected() ||
    !mediaChanClient->isConnected())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "ClientContext(%d) failed to connect to the channels: event, media or frame", id);
    return;
  }

  {
    // Create sender & receiver for event chan.
    eventChanSender = new TrNativeEventSender(eventChanClient);
    eventChanReceiver = new TrNativeEventReceiver(eventChanClient);
  }
  {
    // Create sender & receiver for media chan.
    mediaChanSender = std::make_unique<TrMediaCommandSender>(mediaChanClient);
    mediaChanReceiver = std::make_unique<TrMediaCommandReceiver>(mediaChanClient);
    mediaEventsPollingWorker = std::make_unique<WorkerThread>("TrMediaEventsPolling", [this](WorkerThread &worker)
                                                              {
                                                                media_comm::TrMediaCommandMessage incomingEvent;
                                                                if (mediaChanReceiver->recvCommand(incomingEvent, 100))
                                                                {
                                                                  onListenMediaEvent(incomingEvent);
                                                                } });
  }
  {
    // Create sender & receiver for commandbuffer chan.
    commandBufferChanSender = new TrCommandBufferSender(commandBufferChanClient);
    commandBufferChanReceiver = new TrCommandBufferReceiver(commandBufferChanClient);
  }

  // XR device initialization
  if (xrDeviceInit.enabled && xrDeviceInit.commandChanPort > 0)
  {
    xrCommandChanClient = ipc::TrOneShotClient<xr::TrXRCommandMessage>::MakeAndConnect(xrDeviceInit.commandChanPort, false, id);
    xrCommandChanSender = new xr::TrXRCommandSender(xrCommandChanClient);
    xrCommandChanReceiver = new xr::TrXRCommandReceiver(xrCommandChanClient);
    xrDeviceContextZoneClient = make_unique<xr::TrXRDeviceContextZone>(xrDeviceInit.deviceContextZonePath, TrZoneType::Client);
    xrInputSourcesZoneClient = make_unique<xr::TrXRInputSourcesZone>(xrDeviceInit.inputSourcesZonePath, TrZoneType::Client);

    // Create XR device client
    xrDeviceClient = client_xr::XRDeviceClient::Make(this);
  }

  // Initialize the built-in scene
  builtinScene = builtin_scene::Scene::Make(this);
  if (builtinScene != nullptr)
    builtinScene->bootstrap();

  // Create the window instance
  window = make_shared<::browser::Window>(this);

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
  fprintf(stdout, "ClientContext(%d) mediaChanPort=%d\n", id, mediaChanPort);
  fprintf(stdout, "ClientContext(%d) commandBufferChanPort=%d\n", id, commandBufferChanPort);

  if (xrDeviceInit.enabled == true)
  {
    fprintf(stdout, "ClientContext(%d) xrDeviceInit.active=%s\n", id, xrDeviceInit.active ? "YES" : "NO");
    fprintf(stdout,
            "ClientContext(%d) xrDeviceInit.stereoRenderingMode=%d\n",
            id,
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

bool TrClientContextPerProcess::sendEvent(shared_ptr<TrNativeEvent> event)
{
  return eventChanSender->dispatchEvent(event);
}

TrNativeEventMessage *TrClientContextPerProcess::recvEventMessage(int timeout)
{
  return eventChanReceiver->recvEvent(timeout);
}

shared_ptr<media_client::MediaPlayer> TrClientContextPerProcess::createMediaPlayer(media_comm::MediaContentType contentType)
{
  auto player = make_shared<media_client::MediaPlayer>(contentType);
  mediaPlayers.push_back(player);
  return player;
}

shared_ptr<media_client::AudioPlayer> TrClientContextPerProcess::createAudioPlayer()
{
  auto player = make_shared<media_client::AudioPlayer>();
  mediaPlayers.push_back(dynamic_pointer_cast<media_client::MediaPlayer>(player));
  return player;
}

TrClientContextPerProcess::WebGLContextReference TrClientContextPerProcess::createHostWebGLContext()
{
  client_graphics::ContextAttributes contextAttrs;
  contextAttrs.xrCompatible = true;
  auto newContext = client_graphics::WebGL2Context::Make(contextAttrs);
  assert(newContext != nullptr && newContext->isXRCompatible());

  // Only valid context id is allowed.
  if (newContext->id >= commandbuffers::MinimumContextId)
  {
    hostWebGLContexts.push_back(newContext);
    return newContext;
  }
  else
  {
    return nullptr;
  }
}

TrClientContextPerProcess::WebGLContextReference TrClientContextPerProcess::getHostWebGLContext(uint32_t contextId)
{
  for (auto &context : hostWebGLContexts)
  {
    if (context->id == contextId)
      return context;
  }
  return nullptr;
}

bool TrClientContextPerProcess::removeHostWebGLContext(uint32_t contextId)
{
  for (auto it = hostWebGLContexts.begin(); it != hostWebGLContexts.end(); it++)
  {
    if ((*it)->id == contextId)
    {
      hostWebGLContexts.erase(it);
      return true;
    }
  }
  return false;
}

bool TrClientContextPerProcess::sendCommandBufferRequest(TrCommandBufferBase &commandBuffer, bool followsFlush)
{
  return commandBufferChanSender->sendCommandBufferRequest(commandBuffer, followsFlush);
}

TrCommandBufferResponse *TrClientContextPerProcess::recvCommandBufferResponse(int timeout)
{
  return commandBufferChanReceiver->recvCommandBufferResponse(timeout);
}

void TrClientContextPerProcess::onListenMediaEvent(media_comm::TrMediaCommandMessage &eventMessage)
{
  auto messageType = eventMessage.getType();
  if (messageType == TrMediaCommandType::OnMediaEvent)
  {
    auto mediaEvent = TrMediaCommandBase::CreateFromMessage<TrOnMediaEvent>(eventMessage);
    auto clientId = mediaEvent.clientId;
    for (auto &mediaPlayer : mediaPlayers)
    {
      if (mediaPlayer->id == clientId)
      {
        mediaPlayer->dispatchEvent(mediaEvent.eventType);
        break;
      }
    }
  }
  else if (messageType == TrMediaCommandType::OnMediaMetadata)
  {
    auto metadata = TrMediaCommandBase::CreateFromMessage<TrOnMediaMetadata>(eventMessage);
    auto clientId = metadata.clientId;
    for (auto &mediaPlayer : mediaPlayers)
    {
      if (mediaPlayer->id == clientId)
      {
        mediaPlayer->duration = metadata.duration;
        // TODO: other metadata?
        break;
      }
    }
  }
  else
  {
    fprintf(stderr, "ClientContext(%d) received an unknown media event message\n", id);
  }
}
