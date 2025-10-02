#include "./v8_object_wrap_base.hpp"
#include "./v8_object_wrap.hpp"
#include "./v8_object_holder.hpp"

using namespace std;
using namespace v8;

namespace scripting_base
{
  BaseObject::BaseObject(Isolate *isolate)
      : current_isolate_(isolate)
      , napi_env_(nullptr)
  {
  }

  BaseObject::BaseObject(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : current_isolate_(isolate)
      , napi_env_(nullptr)
  {
  }

  BaseObject::~BaseObject()
  {
    object_handle_.Reset();
  }

  Local<Object> BaseObject::getJSObject(Isolate *isolate) const
  {
    return object_handle_.Get(isolate);
  }

  void BaseObject::setData(std::shared_ptr<JSObjectHolder> handle)
  {
    // Update the data handle.
    data_handle_ = handle;

    // Set the weak reference back to this object.
    if (data_handle_ != nullptr)
      data_handle_->setReference(this);
  }

  void BaseObject::setNapiEnv(napi_env env)
  {
    napi_env_ = env;
  }
}
