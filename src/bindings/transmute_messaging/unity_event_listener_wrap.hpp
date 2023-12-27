#pragma once

#include <map>
#include <mutex>
#include <napi.h>

using namespace std;

namespace messaging
{
  /**
   * This class is used to send event message from Unity to JavaScript runtime.
   */
  class JSEventMessage
  {
  public:
    /**
     * The type of the event message.
     */
    string type;
    /**
     * The data of the event message, which should be serialized via JSON format.
     */
    string data;
  };

  /**
   * This class is used to manage the events channel between the Unity and JavaScript runtime, it provides the ability to send event
   * message from Unity to JavaScript runtime and reverse.
   *
   * The events channel is implemented via a thread safe queue via Node.js ThreadSafe function, the Unity side will push the message
   * to the queue, and the Node.js will dispatch the event to a registered JavaScript callback, in this way, the event message can
   * be sent from Unity to JavaScript runtime.
   */
  class UnityEventListenerWrap : public Napi::ObjectWrap<UnityEventListenerWrap>
  {
  public:
    /**
     * Initialize the UnityEventListenerWrap class and register it to the Node.js runtime.
     */
    static void Init(Napi::Env env, Napi::Object exports);
    /**
     * Get the instance of the UnityEventListenerWrap class, this static method is provided for C++ side to get the instance of thh
     * UnityEventListenerWrap class.
     */
    static UnityEventListenerWrap *GetInstance();

  public:
    /**
     * Node.js constructor.
     */
    UnityEventListenerWrap(const Napi::CallbackInfo &info);

  public:
    /**
     * Dispatch the event message to the JavaScript runtime.
     */
    bool Dispatch(string type, string data);
    /**
     * Convinient method to dispatch the input event message to the JavaScript runtime.
     */
    bool DispatchInputEvent(string data);
    /**
     * Node.js destructor.
     */
    void Finalize(Napi::Env env);

  private:
    /**
     * JS function: set the callback function to receive the event message from Unity.
     */
    Napi::Value SetCallback(const Napi::CallbackInfo &info);
    /**
     * JS function: dispose the UnityEventListenerWrap class from JavaScript side.
     */
    Napi::Value Dispose(const Napi::CallbackInfo &info);

  private:
    /**
     * A flag to mark this instance is disposed or not, dispatching event message to a disposed instance will be ignored.
     */
    bool disposed_;
    /**
     * A thread-safe function instance to dispatch the event message to the JavaScript runtime, this is initialized by `SetCallback()`.
     */
    Napi::ThreadSafeFunction tsfn_;

    /**
     * The global instance of the UnityEventListenerWrap class.
     */
    static UnityEventListenerWrap *instance_;
    /**
     * The constructor of the UnityEventListenerWrap class in JavaScript runtime.
     */
    static Napi::FunctionReference *constructor;
  };
} // namespace messaging
