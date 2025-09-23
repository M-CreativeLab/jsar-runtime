#include "./node_list.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void NodeList::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add properties
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "length").ToLocalChecked(),
                                  LengthGetter);

    // Add methods
    prototypeTemplate->Set(isolate, "item", FunctionTemplate::New(isolate, Item));
    prototypeTemplate->Set(isolate, "entries", FunctionTemplate::New(isolate, Entries));
    prototypeTemplate->Set(isolate, "forEach", FunctionTemplate::New(isolate, ForEach));
    prototypeTemplate->Set(isolate, "keys", FunctionTemplate::New(isolate, Keys));
    prototypeTemplate->Set(isolate, "values", FunctionTemplate::New(isolate, Values));

    // Set up indexed property handlers for array-like access
    instanceTemplate->SetHandler(IndexedPropertyHandlerConfiguration(
      IndexedPropertyGetter,
      nullptr, // No setter - NodeList is read-only
      IndexedPropertyQuery,
      nullptr, // No deleter - NodeList is read-only
      IndexedPropertyEnumerator));
  }

  Local<Object> NodeList::NewInstance(Isolate *isolate, shared_ptr<dom::NodeListApi> nativeList)
  {
    EscapableHandleScope scope(isolate);

    if (nativeList == nullptr)
    {
      return scope.Escape(Local<Object>());
    }
    else
    {
      return scope.Escape(NodeListBase::NewInstance(isolate, nativeList).As<Object>());
    }
  }

  Local<Function> NodeList::Initialize(Isolate *isolate)
  {
    return NodeList::ObjectWrap::Initialize(isolate);
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

    if (info.Length() < 1 || !info[0]->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "NodeList.item requires a numeric index").ToLocalChecked()));
      return;
    }

    uint32_t index = info[0]->Uint32Value(isolate->GetCurrentContext()).FromMaybe(0);
    cout << "NodeList.item(" << index << ") called" << endl;

    // TODO: Return the node at the specified index
    info.GetReturnValue().SetNull();
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
  void NodeList::LengthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "NodeList.length getter called" << endl;
    // TODO: Return the actual length of the node list
    info.GetReturnValue().Set(0);
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