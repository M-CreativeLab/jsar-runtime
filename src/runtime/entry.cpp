#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <node.h>

#include "debug.hpp"
#include "entry.hpp"
#include "jsbundle.h"

#include "bindings/env/env.hpp"
#include "bindings/env/binding.hpp"
#include "bindings/webaudio/audio.hpp"
#include "bindings/transmute_messaging/messaging.hpp"
#include "bindings/transmute_vgom/gom.hpp"

#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
#include <sys/system_properties.h>
#endif

using namespace std;
using namespace node;
using namespace v8;
using namespace gom;
using namespace env;
using namespace messaging;
using namespace webaudio;

node::InitializationResult *processInitializationResult = nullptr;
MultiIsolatePlatform *platformOnProcess = nullptr;
TransmuteLogger *transmuteLoggerOnProcess = nullptr;

bool isDebuggerEnabled = false;
bool isRuntimeVersionUpdated = false;

napi_value WriteStdout(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < 1)
    {
        napi_throw_type_error(env, nullptr, "Missing argument");
        return nullptr;
    }

    size_t bufferLength;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &bufferLength);
    char *bufferData = new char[bufferLength + 1];
    napi_get_value_string_utf8(env, argv[0], bufferData, bufferLength + 1, nullptr);

    const char *data = bufferData;
    transmuteLoggerOnProcess->writeStdout(data);
    return nullptr;
}

napi_value WriteStderr(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < 1)
    {
        napi_throw_type_error(env, nullptr, "Missing argument");
        return nullptr;
    }

    size_t bufferLength;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &bufferLength);
    char *bufferData = new char[bufferLength + 1];
    napi_get_value_string_utf8(env, argv[0], bufferData, bufferLength + 1, nullptr);

    const char *data = bufferData;
    transmuteLoggerOnProcess->writeStderr(data);
    return nullptr;
}

napi_value InitializeTransmuteLogger(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value fn_writestdout, fn_writestderr;
    status = napi_create_function(env, nullptr, 0, WriteStdout, nullptr, &fn_writestdout);
    status = napi_set_named_property(env, exports, "writeStdout", fn_writestdout);
    status = napi_create_function(env, nullptr, 0, WriteStderr, nullptr, &fn_writestderr);
    status = napi_set_named_property(env, exports, "writeStderr", fn_writestderr);
    return exports;
}

static napi_module transmute_logger_napi_mod = {
    NAPI_MODULE_VERSION,
    node::ModuleFlags::kLinked,
    nullptr,
    InitializeTransmuteLogger,
    "transmute:logger",
    nullptr,
    {0},
};

NODE_API_LINKED_MODULE(env, "transmute:env", InitEnvModule);
NODE_API_LINKED_MODULE(messaging, "transmute:messaging", InitMessagingModule);
NODE_API_LINKED_MODULE(gom, "transmute:gom", InitGameObjectModelModule);
NODE_API_LINKED_MODULE(webaudio, "webaudio", InitWebAudioModule);

/**
 * This function is called when the Node.js instance is exit, we need to dispose all the javascript runtime states and handles here.
 */
void NodejsOnExitHandler(Environment *env, int exit_code)
{
    DEBUG("transmute", "NodejsOnExitHandler: exit_code = %d", exit_code);

    // remove `GameObjectModelBufferWritter::keyedBufferWrittersMap` items
    for (auto &pair : GameObjectModelBufferWritter::keyedBufferWrittersMap)
        delete pair.second;
    GameObjectModelBufferWritter::keyedBufferWrittersMap.clear();

    TransmuteNative_Dispose(true);
    node::Stop(env);
}

int RunNodeInstance(MultiIsolatePlatform *platform)
{
    int exit_code = 0;
    const std::vector<std::string> &args = processInitializationResult->args();
    const std::vector<std::string> &exec_args = processInitializationResult->exec_args();

    std::vector<std::string> new_args(args);
    // TODO

    // Setup up a libuv event loop, v8::Isolate, and Node.js Environment.
    std::vector<std::string> errors;
    std::unique_ptr<CommonEnvironmentSetup> setup =
        CommonEnvironmentSetup::Create(platform, &errors, new_args, exec_args);
    if (!setup)
    {
        for (const std::string &err : errors)
            fprintf(stderr, "%s: %s\n", args[0].c_str(), err.c_str());
        return 1;
    }

    Isolate *isolate = setup->isolate();
    Environment *env = setup->env();
    node::SetProcessExitHandler(env, NodejsOnExitHandler);
    DEBUG("transmute", "RunNodeInstance: setup is ready and isolate & environment are created");

    {
        Locker locker(isolate);
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        // The v8::Context needs to be entered when node::CreateEnvironment() and
        // node::LoadEnvironment() are being called.
        Context::Scope context_scope(setup->context());

        // Add the transmute:core module to the Node.js instance.
        AddLinkedBinding(env, transmute_logger_napi_mod);
        AddLinkedBinding(env, transmute_env_napi_mod);
        AddLinkedBinding(env, transmute_gom_napi_mod);
        AddLinkedBinding(env, transmute_messaging_napi_mod);
        AddLinkedBinding(env, transmute_webaudio_napi_mod);

        // Set up the Node.js instance for execution, and run code inside of it.
        // There is also a variant that takes a callback and provides it with
        // the `require` and `process` objects, so that it can manually compile
        // and run scripts as needed.
        // The `require` function inside this script does *not* access the file
        // system, and can only load built-in Node.js modules.
        // `module.createRequire()` is being used to create one that is able to
        // load files from the disk, and uses the standard CommonJS file loader
        // instead of the internal-only `require` function.
        MaybeLocal<Value> loadenv_ret = node::LoadEnvironment(env, node::StartExecutionCallback{});
        if (loadenv_ret.IsEmpty()) // There has been a JS exception.
            return 1;

        exit_code = node::SpinEventLoop(env).FromMaybe(1);
        DEBUG("transmute", "Node.js instance's event loop exited, code=%d", exit_code);

        // node::Stop() can be used to explicitly stop the event loop and keep
        // further JavaScript from running. It can be called from any thread,
        // and will act like worker.terminate() if called from another thread.
        node::Stop(env);
        DEBUG("transmute", "Node.js instance is stopped.");

        if (transmuteLoggerOnProcess != nullptr)
            transmuteLoggerOnProcess->reset();
    }
    return exit_code;
}

class NodeBootstrapper
{
private:
    static NodeBootstrapper *instance_;

public:
    static NodeBootstrapper *GetInstance()
    {
        if (instance_ == nullptr)
            instance_ = new NodeBootstrapper();
        return instance_;
    }

public:
    NodeBootstrapper()
        : threadHandle(nullptr), waitingForCompletion(false)
    {
    }

    ~NodeBootstrapper()
    {
        if (threadHandle != nullptr)
            delete threadHandle;
    }

    std::thread *currentThread() const
    {
        return threadHandle;
    }

    void start()
    {
        if (started == true)
            return;

        // Clear if the `threadHandle` is present.
        if (threadHandle != nullptr && !started)
        {
            if (threadHandle->joinable())
                threadHandle->join();
            delete threadHandle;
        }

        DEBUG("transmute", "created a new thread to run the nodejs script.");
        threadHandle = new std::thread([this]()
                                       {
            if (RunNodeInstance(platformOnProcess) != 0)
                DEBUG("transmute", "nodejs script exited with error.");

            started = false;
            if (dispose_v8_onexit)
            {
                V8::Dispose();
                V8::DisposePlatform();
                node::TearDownOncePerProcess();
            } });

        started = true;
    }

    void waitForCompletion()
    {
        if (waitingForCompletion)
            return;
        waitingForCompletion = true;

        if (threadHandle != nullptr && threadHandle->joinable())
            threadHandle->join();

        // Just deleting this object when join is completed.
        delete this;
    }

    void disposeV8()
    {
        dispose_v8_onexit = true;
    }

private:
    std::thread *threadHandle;
    bool waitingForCompletion;
    bool completing;

    /** mark the node instance is started */
    bool started = false;
    bool dispose_v8_onexit = false;
};

// declare the static member of AsyncExecutingWorker
NodeBootstrapper *NodeBootstrapper::instance_;

extern "C"
{
    /**
     * Initialize the Node.js instance.
     *
     * @param isDebug Whether to enable the debug mode.
     */
    DLL_PUBLIC int TransmuteNative_Initialize(bool isDebug)
    {
        DEBUG("transmute", "Initializing Node.js.");
        if (processInitializationResult != nullptr && platformOnProcess != nullptr)
            return 1;

        std::vector<std::string> args = {
            "node",
            "--experimental-vm-modules",
            "-e",
            jsframework_source};

#ifdef __ANDROID__
        if (isDebug)
        {
            args.insert(args.begin() + 1, "--inspect=0.0.0.0:9229"); // Add --inspect at index 1
        }
#endif

        DEBUG("transmute", "Initializing Node.js with args: %s", args[0].c_str());
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
            processInitializationResult = result.release();
        }

        // Initialize the v8 platform.
        DEBUG("transmute", "[Transmute]: Initializing V8 platform.");
        std::unique_ptr<MultiIsolatePlatform> platform = MultiIsolatePlatform::Create(4);
        V8::InitializePlatform(platform.get());
        V8::Initialize();
        // Convert the unique_ptr to a raw pointer and store it in a global
        platformOnProcess = platform.release();

        // Initialize the `Transmute` components.
        transmuteLoggerOnProcess = TransmuteLogger::GetInstance();
        return 0;
    }

    DLL_PUBLIC void *TransmuteNative_Start()
    {
        if (processInitializationResult == nullptr || platformOnProcess == nullptr)
        {
            DEBUG("transmute", "TransmuteNative_StartAndWaitAvailable: Node.js is not initialized.");
            return nullptr;
        }

        NodeBootstrapper *worker_instance = NodeBootstrapper::GetInstance();
        worker_instance->start();
        void *handle = static_cast<void *>(worker_instance);
        return handle;
    }

    DLL_PUBLIC bool TransmuteNative_IsRuntimeAvailable()
    {
        TransmuteEnvironment *env = TransmuteEnvironment::GetInstance();
        if (env->IsRuntimeAvailable())
        {
            if (!isRuntimeVersionUpdated)
            {
#if defined(__ANDROID__) && (__ANDROID_API__ >= 26)
                auto runtimeVersions = env->GetRuntimeVersions();
                __system_property_set("jsar.runtime.versions", runtimeVersions.c_str());
#endif
                isRuntimeVersionUpdated = true;
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    DLL_PUBLIC void *TransmuteNative_StartAndWaitAvailable()
    {
        void *handle = TransmuteNative_Start();
        TransmuteEnvironment *env = TransmuteEnvironment::GetInstance();
        env->WaitRuntimeAvailable();
        return handle;
    }

    DLL_PUBLIC void TransmuteNative_SetReady(const char *contextJson)
    {
        TransmuteEnvironment *env = TransmuteEnvironment::GetInstance();
        if (env == nullptr)
            return;
        env->SetReadyContext(contextJson);
    }

    DLL_PUBLIC void TransmuteNative_SetFirstFrameReady(const char *uuid)
    {
        UnityEventListenerWrap *listener = UnityEventListenerWrap::GetInstance();
        if (listener != nullptr)
            listener->Dispatch("firstFrameReady", uuid);
    }

    DLL_PUBLIC int TransmuteNative_SetLoggerCallback(LoggerDataCallback callback)
    {
        transmuteLoggerOnProcess->setDataCallback(callback);
        return 0;
    }

    DLL_PUBLIC void TransmuteNative_ExecuteAsync(const char *requestJson)
    {
        UnityEventListenerWrap *listener = UnityEventListenerWrap::GetInstance();
        if (listener != nullptr)
        {
            listener->Dispatch("load", requestJson);
        }
    }

    DLL_PUBLIC void TransmuteNative_WaitAtExecuted(void *workerHandle)
    {
        NodeBootstrapper *worker = static_cast<NodeBootstrapper *>(workerHandle);
        if (worker != nullptr)
            worker->waitForCompletion();
    }

    DLL_PUBLIC bool TransmuteNative_InstantiateGameObjectModelBuffer(const char *uuid)
    {
        if (GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] != nullptr)
            return false;

        GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] = new GameObjectModelBufferWritter();
        return true;
    }

    DLL_PUBLIC void TransmuteNative_DisposeGameObjectModelBuffer(const char *uuid)
    {
        if (GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] != nullptr)
        {
            delete GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid];
            GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] = nullptr;
        }
    }

    DLL_PUBLIC void TransmuteNative_LockGameObjectModelBuffer(const char *uuid)
    {
        if (GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] != nullptr)
            GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid]->lockBuffer();
    }

    DLL_PUBLIC void TransmuteNative_UnlockGameObjectModelBuffer(const char *uuid)
    {
        if (GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] != nullptr)
            GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid]->unlockBuffer();
    }

    DLL_PUBLIC int TransmuteNative_GetGameObjectModelBufferState(const char *uuid)
    {
        if (GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] != nullptr)
            return (int)GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid]->bufferState();
        else
            return -1;
    }

    DLL_PUBLIC int TransmuteNative_GetGameObjectModelBufferSize(const char *uuid)
    {
        if (GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] != nullptr)
            return GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid]->bufferSize();
        else
            return 0;
    }

    DLL_PUBLIC void TransmuteNative_LoadGameObjectModelBuffer(const char *uuid, uint8_t *dest)
    {
        if (GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid] != nullptr)
            GameObjectModelBufferWritter::keyedBufferWrittersMap[uuid]->copyBuffer(dest);
    }

    DLL_PUBLIC void TransmuteNative_DispatchContextEvent(const char *dataJson)
    {
        UnityEventListenerWrap *listener = UnityEventListenerWrap::GetInstance();
        if (listener != nullptr)
            listener->Dispatch("context", dataJson);
    }

    DLL_PUBLIC void TransmuteNative_DispatchAnimationEvent(const char *dataJson)
    {
        UnityEventListenerWrap *listener = UnityEventListenerWrap::GetInstance();
        if (listener != nullptr)
            listener->Dispatch("animation", dataJson);
    }

    DLL_PUBLIC bool TransmuteNative_DispatchInputEvent(const char *dataJson)
    {
        UnityEventListenerWrap *listener = UnityEventListenerWrap::GetInstance();
        if (listener != nullptr)
            return listener->DispatchInputEvent(dataJson);

        // DEBUG("transmute", "Skipped an input event dispatching, UnityEventListenerWrap is not initialized.");
        return true; /** FIXME(yorkie): just mark as it's called, because we don't want an event effect the system. */
    }

    DLL_PUBLIC int TransmuteNative_Dispose(bool perserveV8)
    {
        UnityEventListenerWrap *listener = UnityEventListenerWrap::GetInstance();
        if (listener != nullptr)
        {
            listener->Dispatch("dispose", "");
            DEBUG("transmute", "Send a \"dispose\" event to Node.js runtime");
        }

        TransmuteEnvironment::GetInstance()->Reset();
        isRuntimeVersionUpdated = false;

        // Dispose v8 later.
        if (!perserveV8)
        {
            /**
             * We cannot directly dispose the v8 and platforms here, this function just sets a flag and when Node.js exits,
             * it will read the flag and dispose the v8 and platforms.
             */
            NodeBootstrapper::GetInstance()->disposeV8();
        }
        return 0;
    }
}
