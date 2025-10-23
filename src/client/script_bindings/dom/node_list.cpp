#include <iostream>
#include "./node_list.hpp"
#include "./node.hpp"

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  Local<Value> NodeList::NodeListIterator::createNextValue(Isolate *isolate,
                                                           const std::shared_ptr<dom::Node> value)
  {
    EscapableHandleScope scope(isolate);
    if (value != nullptr)
    {
      Local<Object> nodeWrapper = Node::GetOrNewInstance(isolate, value);
      return scope.Escape(nodeWrapper);
    }
    else
    {
      return scope.Escape(Null(isolate));
    }
  }

  // static
  Local<Function> NodeList::Initialize(Isolate *isolate)
  {
    NodeListIterator::Initialize(isolate);
    return NodeListBase::Initialize(isolate);
  }

  // static
  void NodeList::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    // Add properties
    InstanceReadonlyAccessor(isolate, instanceTemplate, "length", &NodeList::LengthGetter);

    // Add methods
    InstanceMethod(isolate, instanceTemplate, "item", &NodeList::Item);
    InstanceMethod(isolate, instanceTemplate, "entries", &NodeList::Entries);
    InstanceMethod(isolate, instanceTemplate, "forEach", &NodeList::ForEach);
    InstanceMethod(isolate, instanceTemplate, "keys", &NodeList::Keys);
    InstanceMethod(isolate, instanceTemplate, "values", &NodeList::Values);
    InstanceMethod(isolate, instanceTemplate, Symbol::GetIterator(isolate), &NodeList::Values);

    // Set up indexed property handlers for array-like access
    instanceTemplate->SetHandler(IndexedPropertyHandlerConfiguration(
      IndexedPropertyGetter,
      IndexedPropertySetter,
      nullptr,
      IndexedPropertyDeleter,
      IndexedPropertyEnumerator));
  }

  NodeList::NodeList(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : NodeListBase(isolate, args)
  {
    // NodeList constructor
  }

  // Method implementations
  void NodeList::Item(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1 || !info[0]->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "item", "1 argument required, but only 0 present.")));
      return;
    }

    uint32_t index = info[0]->Uint32Value(context).FromMaybe(0);
    auto node = handle()->item(index);
    if (node != nullptr)
    {
      Local<Object> nodeWrapper = Node::GetOrNewInstance(isolate, node);
      info.GetReturnValue().Set(nodeWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void NodeList::Entries(const FunctionCallbackInfo<Value> &info)
  {
    cout << "NodeList.entries called" << endl;
    // TODO: Return an iterator of [index, node] pairs
    info.GetReturnValue().SetUndefined();
  }

  void NodeList::ForEach(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();

    if (info.Length() < 1 || !info[0]->IsFunction())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "NodeList.forEach requires a callback function").ToLocalChecked()));
      return;
    }

    cout << "NodeList.forEach called" << endl;

    // TODO: Call the callback for each node in the list
    info.GetReturnValue().SetUndefined();
  }

  void NodeList::Keys(const FunctionCallbackInfo<Value> &info)
  {
    cout << "NodeList.keys called" << endl;
    // TODO: Return an iterator of indices
    info.GetReturnValue().SetUndefined();
  }

  void NodeList::Values(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto values = NodeListIterator::NewInstance(nullptr, *handle());
    info.GetReturnValue().Set(values);
  }

  // Property getters
  void NodeList::LengthGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    int length = handle()->length();
    info.GetReturnValue().Set(Integer::New(isolate, length));
  }

  // Indexed property handlers
  void NodeList::IndexedPropertyGetter(uint32_t index, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    NodeList *wrapper = NodeList::Unwrap(isolate, info.This());
    if (wrapper == nullptr || !wrapper->hasData())
    {
      info.GetReturnValue().SetUndefined();
      return;
    }
    else
    {
      auto node = wrapper->handle()->item(index);
      if (node != nullptr)
      {
        Local<Object> nodeWrapper = Node::GetOrNewInstance(isolate, node);
        info.GetReturnValue().Set(nodeWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }
  }

  void NodeList::IndexedPropertySetter(uint32_t index, Local<Value> value, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "NodeList is read-only").ToLocalChecked()));
  }

  void NodeList::IndexedPropertyDeleter(uint32_t index, const PropertyCallbackInfo<Boolean> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "NodeList is read-only").ToLocalChecked()));
  }

  void NodeList::IndexedPropertyEnumerator(const PropertyCallbackInfo<Array> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Array> keys = Array::New(isolate, 0);
    NodeList *instance = Unwrap(isolate, info.This());
    if (instance != nullptr && instance->hasData())
    {
      auto len = instance->handle()->length();
      for (size_t index = 0; index < len; index++)
        keys->Set(context, index, Integer::New(isolate, index)).FromJust();
    }
    info.GetReturnValue().Set(keys);
  }
}
