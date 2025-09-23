#include "./mutation_observer.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  void MutationObserver::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add methods
    prototypeTemplate->Set(isolate, "observe",
                          FunctionTemplate::New(isolate, Observe));
    prototypeTemplate->Set(isolate, "disconnect",
                          FunctionTemplate::New(isolate, Disconnect));
    prototypeTemplate->Set(isolate, "takeRecords",
                          FunctionTemplate::New(isolate, TakeRecords));
  }

  Local<Object> MutationObserver::NewInstance(Isolate *isolate, shared_ptr<dom::MutationObserver> nativeObserver)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Function> constructor = MutationObserver::GetConstructorFunction(isolate);
    Local<Object> instance = constructor->NewInstance(context, 0, nullptr).ToLocalChecked();

    MutationObserver *wrapper = new MutationObserver(isolate, *reinterpret_cast<const FunctionCallbackInfo<Value> *>(&instance));
    wrapper->SetNativeInstance(nativeObserver);
    MutationObserver::Wrap(isolate, instance, wrapper);

    return instance;
  }

  Local<Function> MutationObserver::Initialize(Isolate *isolate)
  {
    return MutationObserver::ObjectWrap::Initialize(isolate);
  }

  MutationObserver::MutationObserver(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
    : MutationObserverBase(isolate, args)
  {
    // MutationObserver constructor
    // TODO: Store the callback function from args[0]
    if (args.Length() < 1 || !args[0]->IsFunction())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "MutationObserver constructor requires a callback function").ToLocalChecked()));
      return;
    }
    cout << "MutationObserver constructor called with callback" << endl;
  }

  // Method implementations
  void MutationObserver::Observe(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "MutationObserver.observe requires at least 1 argument: target").ToLocalChecked()));
      return;
    }

    cout << "MutationObserver.observe called" << endl;
    
    // TODO: Implement observe functionality
    // Arguments: target (Node), options (MutationObserverInit optional)
    
    info.GetReturnValue().SetUndefined();
  }

  void MutationObserver::Disconnect(const FunctionCallbackInfo<Value> &info)
  {
    cout << "MutationObserver.disconnect called" << endl;
    
    // TODO: Implement disconnect functionality
    // Stop observing all target nodes
    
    info.GetReturnValue().SetUndefined();
  }

  void MutationObserver::TakeRecords(const FunctionCallbackInfo<Value> &info)
  {
    cout << "MutationObserver.takeRecords called" << endl;
    
    // TODO: Implement takeRecords functionality
    // Return array of MutationRecord objects and clear the record queue
    
    info.GetReturnValue().Set(Array::New(info.GetIsolate()));
  }
}