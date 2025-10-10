#pragma once

#include <cassert>
#include <memory>
#include <node/v8.h>
#include <node/uv.h>

namespace scripting_base
{
  /**
   * A utility class to facilitate thread-safe calls to JavaScript functions from other threads.
   */
  class ThreadSafeFunction
  {
    using CustomCallback = std::function<void(v8::Isolate *,
                                              v8::Local<v8::Value>,
                                              v8::Local<v8::Function> js_callback)>;

  public:
    ThreadSafeFunction(v8::Isolate *isolate,
                       v8::Local<v8::Value> recv,
                       v8::Local<v8::Function> js_callback);
    ~ThreadSafeFunction();

  public:
    /**
     * Make a non-blocking call to the JavaScript function.
     * 
     * @param custom_callback Optional custom callback to be invoked instead of the original JavaScript function.
     * @note The custom callback if provided will be used to customize the arguments passed to the JavaScript function,
     *       and caller is responsible to manage the lifetime of the `ThreadSafeFunction` instance.
     */
    void nonBlockingCall(CustomCallback custom_callback = nullptr);

  private:
    bool handleCallRequest();

  private:
    v8::Isolate *isolate_;
    v8::Global<v8::Value> js_recv_;
    v8::Global<v8::Function> js_callback_;
    std::optional<CustomCallback> custom_callback_;
    std::unique_ptr<uv_async_t> async_handle_;
  };
}
