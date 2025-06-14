#include <client/scripting_base/v8_utils.hpp>

#include "./node-inl.hpp"
#include "./node_list.hpp"

namespace dombinding
{
  v8::Local<v8::Value> NodeList::NodeListIterator::createNextValue(v8::Isolate *isolate,
                                                                   const std::shared_ptr<dom::Node> value)
  {
    v8::EscapableHandleScope scope(this->current_isolate_);
    Napi::Object valueObject = Node::NewInstance(this->napi_env_, value);
    return scope.Escape(scripting_base::ToV8Local<v8::Value>(valueObject));
  }

  void NodeList::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    v8::Isolate *isolate = v8::Isolate::GetCurrent();

    NodeListIterator::Initialize(isolate); // Initialize the NodeListIterator class
    v8::Local<v8::Function> constructor = Base::Initialize(isolate);

    env.Global().Set(Napi::String::New(env, "NodeList"), scripting_base::Value(constructor));
  }

  Napi::Value NodeList::NewInstance(Napi::Env env, shared_ptr<dom::NodeListApi> list)
  {
    Napi::EscapableHandleScope scope(env);
    v8::Local<v8::Value> jsValue = Base::NewInstance(env, list);

    // Convert the v8::Value to a Napi::Value
    napi_value value;
    memcpy(&value, &jsValue, sizeof(jsValue));
    return scope.Escape(value);
  }

  void NodeList::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    Base::ConfigureFunctionTemplate(isolate, tpl);

    tpl->InstanceTemplate()->Set(isolate, "values", v8::FunctionTemplate::New(isolate, Values));
    tpl->InstanceTemplate()->Set(v8::Symbol::GetIterator(isolate), v8::FunctionTemplate::New(isolate, Values));
    // TODO: support other methods: keys, entries, forEach, item.
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "length").ToLocalChecked(),
                                                 v8::FunctionTemplate::New(isolate, LengthGetter),
                                                 v8::FunctionTemplate::New(isolate, LengthSetter));
    tpl->InstanceTemplate()->SetHandler(v8::IndexedPropertyHandlerConfiguration(
      PropertyGetter,         // Getter
      PropertySetter,         // Setter
      nullptr,                // Query
      PropertyDeleter,        // Deleter
      PropertyEnumerator,     // Enumerator
      nullptr,                // Definer
      nullptr,                // Descriptor
      v8::Local<v8::Value>(), // Data
      v8::PropertyHandlerFlags::kNone));
  }

  void NodeList::PropertyGetter(unsigned int index, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    NodeList *instance = Unwrap(info.This());
    if (!instance)
      info.GetReturnValue().Set(Undefined(isolate));
    else
      info.GetReturnValue().Set(instance->get(index));
  }

  void NodeList::PropertySetter(unsigned int index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    isolate->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8Literal(isolate, "Setting properties on NodeList is not implemented yet.")));
  }

  void NodeList::PropertyDeleter(unsigned int index, const v8::PropertyCallbackInfo<v8::Boolean> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    isolate->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8Literal(isolate, "Deleting properties on NodeList is not implemented yet.")));
  }

  void NodeList::PropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    v8::Local<v8::Array> keys = v8::Array::New(isolate, 0);
    NodeList *instance = Unwrap(info.Holder());
    if (instance != nullptr && instance->hasList())
    {
      auto len = instance->listRef().length();
      for (size_t index = 0; index < len; index++)
        keys->Set(context, index, v8::Integer::New(isolate, index)).FromJust();
    }
    info.GetReturnValue().Set(keys);
  }

  void NodeList::LengthGetter(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    NodeList *instance = Unwrap(info.Holder());
    if (!instance)
      info.GetReturnValue().Set(v8::Integer::New(isolate, 0));
    else
      info.GetReturnValue().Set(v8::Integer::New(isolate, instance->listRef().length()));
  }

  void NodeList::LengthSetter(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    isolate->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8Literal(isolate, "Setting the length property on NodeList is not allowed.")));
  }

  void NodeList::Values(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    NodeList *instance = Unwrap(info.This());
    if (!instance)
      info.GetReturnValue().Set(v8::Undefined(isolate));
    else
      info.GetReturnValue().Set(instance->values());
  }

  v8::Local<v8::Value> NodeList::get(unsigned int index) const
  {
    v8::EscapableHandleScope scope(current_isolate_);
    std::shared_ptr<dom::Node> value = listRef().item(index);
    if (value == nullptr)
      return scope.Escape(v8::Undefined(current_isolate_));

    Napi::Object valueObject = Node::NewInstance(napi_env_, value);
    return scope.Escape(scripting_base::ToV8Local<v8::Value>(valueObject));
  }
}
