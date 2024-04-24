#include <stdlib.h>
#include "node_bootstrapper.hpp"
#include "debug.hpp"
#include "crates/jsar_jsbindings.h"

using namespace v8;
using namespace node;

void nodejs_onexit(node::Environment *env, int exit_code)
{
  DEBUG("transmute", "NodejsOnExitHandler: exit_code = %d", exit_code);
  // TransmuteNative_Dispose(true);
  node::Stop(env);
}

NodeBootstrapper *NodeBootstrapper::instance_ = nullptr;

NodeBootstrapper::NodeBootstrapper() : m_thread(nullptr)
{
}

NodeBootstrapper::~NodeBootstrapper()
{
  if (m_thread != nullptr)
  {
    delete m_thread;
  }
}

bool NodeBootstrapper::initialize()
{
  if (m_nodeInitRes != nullptr && m_nodePlatform != nullptr)
    return true;
  DEBUG("transmute", "start initializing Node.js");

  const char *jsbundle_source = reinterpret_cast<const char *>(get_jsbundle_ptr());
  size_t jsbundle_size = get_jsbundle_size();

  std::vector<std::string> args = {
      "node",
      "--experimental-vm-modules",
      "--experimental-global-customevent",
      "-e",
      std::string(jsbundle_source, jsbundle_size)};

  args.insert(args.begin() + 1, "--inspect=9229");
  // if (isDebug)
  // {
  //   args.insert(args.begin() + 1, "--inspect=9229");
  // }

  DEBUG("transmute", "initializing Node.js with args: %s", args[0].c_str());
  std::unique_ptr<node::InitializationResult> result =
      node::InitializeOncePerProcess(args, {node::ProcessInitializationFlags::kNoInitializeV8,
                                            node::ProcessInitializationFlags::kNoInitializeNodeV8Platform,
                                            node::ProcessInitializationFlags::kNoPrintHelpOrVersionOutput,
                                            node::ProcessInitializationFlags::kNoDefaultSignalHandling});

  for (const std::string &error : result->errors())
    DEBUG("transmute", "%s: %s\n", args[0].c_str(), error.c_str());

  if (result->early_return() != 0)
  {
    DEBUG("transmute", "Early return: %d", result->early_return());
    return result->exit_code();
  }
  else
  {
    // Convert the unique_ptr to a raw pointer and store it in a global
    m_nodeInitRes = result.release();
  }

  // Initialize the v8 platform.
  DEBUG("transmute", "[Transmute]: Initializing V8 platform.");
  std::unique_ptr<node::MultiIsolatePlatform> platform = node::MultiIsolatePlatform::Create(4);
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();
  // Convert the unique_ptr to a raw pointer and store it in a global
  m_nodePlatform = platform.release();
  return true;
}

bool NodeBootstrapper::start()
{
  if (m_running)
    return false;

  // Clear if the `threadHandle` is present.
  if (m_thread != nullptr && m_started == false)
  {
    if (m_thread->joinable())
      m_thread->join();
    delete m_thread;
    m_thread = nullptr;
  }

  DEBUG("transmute", "created a new thread to run the nodejs script.");
  m_thread = new std::thread([this]()
                             {
            if (runNodeInstance() != 0)
              DEBUG("transmute", "nodejs script exited with error.");

            m_running = false;
            if (m_disposeV8OnExit)
            {
              V8::Dispose();
              V8::DisposePlatform();
              node::TearDownOncePerProcess();
            } });
  m_started = true;
  m_running = true;
  return true;
}

bool NodeBootstrapper::isRunning()
{
  return m_running;
}

TransmuteEnvironment *NodeBootstrapper::getEnv()
{
  return TransmuteEnvironment::GetInstance();
}

bool NodeBootstrapper::isRuntimeAvailable()
{
  return getEnv()->isRuntimeAvailable();
}

int NodeBootstrapper::runNodeInstance()
{
  SET_THREAD_NAME("JSARNode");

  int exit_code = 0;
  const std::vector<std::string> &args = m_nodeInitRes->args();
  const std::vector<std::string> &exec_args = m_nodeInitRes->exec_args();

  std::vector<std::string> new_args(args);
  // TODO

  // Setup up a libuv event loop, v8::Isolate, and Node.js Environment.
  std::vector<std::string> errors;
  std::unique_ptr<node::CommonEnvironmentSetup> setup =
      node::CommonEnvironmentSetup::Create(m_nodePlatform, &errors, new_args, exec_args);
  if (!setup)
  {
    for (const std::string &err : errors)
      fprintf(stderr, "%s: %s\n", args[0].c_str(), err.c_str());
    return 1;
  }

  v8::Isolate *isolate = setup->isolate();
  node::Environment *env = setup->env();
  node::SetProcessExitHandler(env, nodejs_onexit);

  {
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    Context::Scope context_scope(setup->context());

    // Add the transmute:core module to the Node.js instance.
    AddLinkedBinding(env, transmute_env_napi_mod);
    AddLinkedBinding(env, transmute_logger_napi_mod);
    AddLinkedBinding(env, transmute_messaging_napi_mod);
    AddLinkedBinding(env, transmute_renderer_napi_mod);
    // AddLinkedBinding(env, transmute_webaudio_napi_mod);
    AddLinkedBinding(env, transmute_webgl_napi_mod);
    AddLinkedBinding(env, transmute_webxr_napi_mod);
    // The followings are created by Rust
    AddLinkedBinding(env, transmute_htmlrender_napi_mod);

    MaybeLocal<Value> loadenv_ret = node::LoadEnvironment(env, node::StartExecutionCallback{});
    if (loadenv_ret.IsEmpty()) // There has been a JS exception.
      return 1;

    exit_code = node::SpinEventLoop(env).FromMaybe(1);
    node::Stop(env);
    SET_THREAD_NAME("JSARNode_Stopped");

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
