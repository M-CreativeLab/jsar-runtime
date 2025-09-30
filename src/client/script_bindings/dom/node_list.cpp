#include <iostream>
#include "./node_list.hpp"
#include "./node.hpp"

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void NodeList::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add properties
    InstanceReadonlyAccessor(isolate, instanceTemplate, "length", &NodeList::LengthGetter);

    // Add methods
    InstanceMethod(isolate, prototypeTemplate, "item", &NodeList::Item);
    InstanceMethod(isolate, prototypeTemplate, "entries", &NodeList::Entries);
    InstanceMethod(isolate, prototypeTemplate, "forEach", &NodeList::ForEach);
    InstanceMethod(isolate, prototypeTemplate, "keys", &NodeList::Keys);
    InstanceMethod(isolate, prototypeTemplate, "values", &NodeList::Values);

    // Set up indexed property handlers for array-like access
    instanceTemplate->SetHandler(IndexedPropertyHandlerConfiguration(
      IndexedPropertyGetter,
      nullptr, // No setter - NodeList is read-only
      IndexedPropertyQuery,
      nullptr, // No deleter - NodeList is read-only
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
    cout << "NodeList.values called" << endl;
    // TODO: Return an iterator of nodes
    info.GetReturnValue().SetUndefined();
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
    cout << "NodeList[" << index << "] getter called" << endl;
    // TODO: Return the node at the specified index
    info.GetReturnValue().SetNull();
  }

  void NodeList::IndexedPropertyQuery(uint32_t index, const PropertyCallbackInfo<Integer> &info)
  {
    cout << "NodeList[" << index << "] query called" << endl;
    // TODO: Check if index is valid
    info.GetReturnValue().Set(Integer::New(info.GetIsolate(), None));
  }

  void NodeList::IndexedPropertyEnumerator(const PropertyCallbackInfo<Array> &info)
  {
    cout << "NodeList indexed enumerator called" << endl;
    // TODO: Return array of valid indices
    info.GetReturnValue().Set(Array::New(info.GetIsolate()));
  }
}