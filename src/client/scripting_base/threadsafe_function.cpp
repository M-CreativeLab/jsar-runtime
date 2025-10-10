#include <client/per_process.hpp>
#include "./threadsafe_function.hpp"

using namespace std;
using namespace v8;

namespace scripting_base
{
  ThreadSafeFunction::ThreadSafeFunction(Isolate *isolate, Local<Value> recv, Local<Function> js_callback)
      : isolate_(isolate)
      , js_recv_(isolate, recv)
      , js_callback_(isolate, js_callback)
      , custom_callback_(nullopt)
      , async_handle_(make_unique<uv_async_t>())
  {
    auto client_context = TrClientContextPerProcess::Get();
    auto loop = client_context->getScriptingEventLoop();
    assert(loop != nullptr && "Scripting event loop is not setup");

    auto async_cb = [](uv_async_t *handle)
    {
      ThreadSafeFunction *tsfn = static_cast<ThreadSafeFunction *>(handle->data);
      assert(tsfn != nullptr && "ThreadSafeFunction data is null");
      if (tsfn->handleCallRequest())
      {
        delete tsfn;
      }
    };
    async_handle_->data = this;
    uv_async_init(loop, async_handle_.get(), async_cb);
  }

  ThreadSafeFunction::~ThreadSafeFunction()
  {
    if (async_handle_ != nullptr)
    {
      auto close_cb = [](uv_handle_t *handle)
      {
        if (handle != nullptr)
          delete handle;
      };
      uv_close(reinterpret_cast<uv_handle_t *>(async_handle_.release()), close_cb);
    }
    js_recv_.Reset();
    js_callback_.Reset();
  }

  void ThreadSafeFunction::nonBlockingCall(CustomCallback custom_callback)
  {
    if (custom_callback != nullptr)
      custom_callback_ = custom_callback;
    uv_async_send(async_handle_.get());
  }

  bool ThreadSafeFunction::handleCallRequest()
  {
    HandleScope scope(isolate_);
    Local<Context> context = isolate_->GetCurrentContext();
    Local<Value> recv = js_recv_.Get(isolate_);
    Local<Function> callback = js_callback_.Get(isolate_);

    if (custom_callback_)
    {
      const auto &custom_callback = custom_callback_.value();
      custom_callback(isolate_, recv, callback);
      custom_callback_ = nullopt;
      return false;
    }
    else
    {
      callback->Call(context, recv, 0, nullptr).ToLocalChecked();
      return true;
    }
  }
}
