#pragma once

#include <client/scripting_base/v8_utils.hpp>

#include "./node_list.hpp"
#include "./node.hpp"

namespace dombinding
{
  template <typename TNode>
    requires is_node_type<TNode>
  v8::Local<v8::Value> ArrayIterator<TNode>::createNextValue(v8::Isolate *isolate,
                                                             const std::shared_ptr<TNode> value)
  {
    v8::EscapableHandleScope scope(this->currentIsolate_);
    Napi::Object valueObject = Node::NewInstance(napiEnv_, value);
    return scope.Escape(scripting_base::ToV8Local<v8::Value>(valueObject));
  }

  template <typename TNode>
    requires is_node_type<TNode>
  Napi::Value NodeList<TNode>::NewInstance(Napi::Env env, dom::NodeList<TNode> &list)
  {
    Napi::EscapableHandleScope scope(env);
    v8::Local<v8::Value> jsValue = scripting_base::ObjectWrap<T>::NewInstance(v8::Isolate::GetCurrent(), env, list);

    // Convert the v8::Value to a Napi::Value
    napi_value value;
    memcpy(&value, &jsValue, sizeof(jsValue));
    return scope.Escape(value);
  }

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    scripting_base::ObjectWrap<T>::ConfigureFunctionTemplate(isolate, tpl);

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

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::PropertyGetter(unsigned int index, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    T *instance = Unwrap(info.This());
    if (!instance)
      info.GetReturnValue().Set(Undefined(isolate));
    else
      info.GetReturnValue().Set(instance->get(index));
  }

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::PropertySetter(unsigned int index, v8::Local<v8::Value> value,
                                       const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "Setting properties on NodeList is not implemented yet.")));
  }

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::PropertyDeleter(unsigned int index, const v8::PropertyCallbackInfo<v8::Boolean> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "Deleting properties on NodeList is not implemented yet.")));
  }

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::PropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    v8::Local<v8::Array> keys = v8::Array::New(isolate, 0);
    T *instance = Unwrap(info.Holder());
    if (instance != nullptr)
    {
      auto len = instance->list_.size();
      for (size_t index = 0; index < len; index++)
        keys->Set(context, index, v8::Integer::New(isolate, index)).FromJust();
    }
    info.GetReturnValue().Set(keys);
  }

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::LengthGetter(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    T *instance = Unwrap(info.Holder());
    if (!instance)
      info.GetReturnValue().Set(v8::Integer::New(isolate, 0));
    else
      info.GetReturnValue().Set(v8::Integer::New(isolate, instance->list_.size()));
  }

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::LengthSetter(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "Setting the length property on NodeList is not allowed.")));
  }

  template <typename TNode>
    requires is_node_type<TNode>
  void NodeList<TNode>::Values(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    T *instance = Unwrap(info.This());
    if (!instance)
      info.GetReturnValue().Set(v8::Undefined(isolate));
    else
      info.GetReturnValue().Set(instance->values());
  }

  template <typename TNode>
    requires is_node_type<TNode>
  v8::Local<v8::Value> NodeList<TNode>::get(unsigned int index) const
  {
    v8::EscapableHandleScope scope(this->currentIsolate_);
    std::shared_ptr<TNode> value = list_[index];
    if (value == nullptr)
      return scope.Escape(v8::Undefined(this->currentIsolate_));

    Napi::Object valueObject = Node::NewInstance(napiEnv_, value);
    return scope.Escape(scripting_base::ToV8Local<v8::Value>(valueObject));
  }
}
