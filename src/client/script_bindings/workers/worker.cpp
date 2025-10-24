#include <client/script_bindings/events/message_event.hpp>

#include "./worker.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings::workers_bindings
  {
    Persistent<Function> Worker::WorkerImplConstructor;
    void Worker::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add methods
      {
        InstanceMethod(isolate, prototype, "postMessage", &Worker::PostMessage);
        InstanceMethod(isolate, prototype, "terminate", &Worker::Terminate);
      }

      // Load the WorkerImpl constructor from the main context
      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> global = context->Global();
      Local<Value> workerImplValue;
      if (global->Get(context, String::NewFromUtf8(isolate, "__WorkerImpl").ToLocalChecked())
            .ToLocal(&workerImplValue) &&
          workerImplValue->IsFunction())
      {
        WorkerImplConstructor.Reset(isolate, workerImplValue.As<Function>());
      }
      else
      {
        assert(false && "`__WorkerImpl` constructor is required for Worker.");
      }
    }

    Worker::Worker(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WorkerBase(isolate, args)
    {
      if (WorkerImplConstructor.IsEmpty())
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "Worker", "`__WorkerImpl` constructor is not available")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      if (args.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodArgCountError(isolate, "Worker", 1, args.Length())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      if (!args[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodArgTypeError(isolate, "Worker", "url", "string", args[0])));
        args.GetReturnValue().SetUndefined();
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      String::Utf8Value url(isolate, args[0]->ToString(context).ToLocalChecked());
      Local<Value> workerImplInstanceValue;
      Local<Function> workerImplConstructor = WorkerImplConstructor.Get(isolate);

      Local<Value> constructorArgs[] = {args[0]};
      if (!workerImplConstructor->NewInstance(context, 1, constructorArgs)
             .ToLocal(&workerImplInstanceValue) ||
          !workerImplInstanceValue->IsObject())
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "Worker", "Failed to create WorkerImpl instance")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      Local<Object> workerImplInstance = workerImplInstanceValue.As<Object>();
      {
        // Configure event handlers
        workerImplInstance->Set(context,
                                String::NewFromUtf8(isolate, "onmessage").ToLocalChecked(),
                                Function::New(context, OnMessage).ToLocalChecked())
          .ToChecked();
        workerImplInstance->Set(context,
                                String::NewFromUtf8(isolate, "onmessageerror").ToLocalChecked(),
                                Function::New(context, OnMessageError).ToLocalChecked())
          .ToChecked();
        workerImplInstance->Set(context,
                                String::NewFromUtf8(isolate, "onerror").ToLocalChecked(),
                                Function::New(context, OnError).ToLocalChecked())
          .ToChecked();

        // Configure `_workerInstance` property
        workerImplInstance->Set(context,
                                String::NewFromUtf8(isolate, "_workerInstance").ToLocalChecked(),
                                args.This())
          .ToChecked();
      }
      worker_impl_handle_.Reset(isolate, workerImplInstance);
      setData(make_shared<client_workers::Worker>(string(*url)));

      {
        // Call `.start()` method to start the worker
        Local<Value> startMethodValue;
        if (workerImplInstance->Get(context, String::NewFromUtf8(isolate, "start").ToLocalChecked())
              .ToLocal(&startMethodValue) &&
            startMethodValue->IsFunction())
        {
          Local<Function> startFunction = startMethodValue.As<Function>();
          Local<Value> startArgs[] = {};
          startFunction->Call(context, workerImplInstance, 0, startArgs).ToLocalChecked();
        }
        else
        {
          isolate->ThrowException(Exception::Error(
            MakeMethodError(isolate, "Worker", "`WorkerImpl.start` method is not available")));
          args.GetReturnValue().SetUndefined();
          return;
        }
      }
    }

    void Worker::onCreated()
    {
      EventTarget::onCreated();

      // Register common events
      registerEvent("message");
      registerEvent("messageerror");
      registerEvent("error");
    }

    void Worker::PostMessage(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (worker_impl_handle_.IsEmpty())
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "postMessage", "`WorkerImpl` instance is not available")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      if (args.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodArgCountError(isolate, "postMessage", 1, args.Length())));
        args.GetReturnValue().SetUndefined();
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> workerImplInstance = worker_impl_handle_.Get(isolate);
      Local<Value> postMessageValue;
      if (!workerImplInstance->Get(context, String::NewFromUtf8(isolate, "postMessage").ToLocalChecked())
             .ToLocal(&postMessageValue) ||
          !postMessageValue->IsFunction())
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "postMessage", "`WorkerImpl.postMessage` method is not available")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      Local<Function> postMessageFunction = postMessageValue.As<Function>();
      Local<Value> postMessageArgs[] = {args[0]};
      postMessageFunction->Call(context, workerImplInstance, 1, postMessageArgs).ToLocalChecked();
      args.GetReturnValue().SetUndefined();
    }

    void Worker::Terminate(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (worker_impl_handle_.IsEmpty())
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "terminate", "`WorkerImpl` instance is not available")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> workerImplInstance = worker_impl_handle_.Get(isolate);
      Local<Value> terminateValue;
      if (!workerImplInstance->Get(context, String::NewFromUtf8(isolate, "terminate").ToLocalChecked())
             .ToLocal(&terminateValue) ||
          !terminateValue->IsFunction())
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "terminate", "`WorkerImpl.terminate` method is not available")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      Local<Function> terminateFunction = terminateValue.As<Function>();
      Local<Value> terminateArgs[] = {};
      terminateFunction->Call(context, workerImplInstance, 0, terminateArgs).ToLocalChecked();
      args.GetReturnValue().SetUndefined();
    }

    void Worker::OnMessage(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Worker *instance;
      string error;
      if (!ValidateAndUnwrap(args, &instance, error))
      {
        cerr << "Worker::OnMessage: " << error << endl;
        args.GetReturnValue().SetUndefined();
        return;
      }

      if (args.Length() < 1 || !args[0]->IsObject()) [[unlikely]]
      {
        cerr << "Worker::OnMessage: Invalid arguments" << endl;
        args.GetReturnValue().SetUndefined();
        return;
      }

      auto messageEvent = CreateMessageEvent(isolate, args[0].As<Object>());
      instance->handle()->dispatchEvent(messageEvent);
    }

    void Worker::OnMessageError(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Worker *instance;
      string error;
      if (!ValidateAndUnwrap(args, &instance, error))
      {
        cerr << "Worker::OnMessageError: " << error << endl;
        args.GetReturnValue().SetUndefined();
        return;
      }

      if (args.Length() < 1 || !args[0]->IsObject()) [[unlikely]]
      {
        cerr << "Worker::OnMessageError: Invalid arguments" << endl;
        args.GetReturnValue().SetUndefined();
        return;
      }

      auto messageEvent = CreateMessageEvent(isolate, args[0].As<Object>());
      instance->handle()->dispatchEvent(messageEvent);
    }

    void Worker::OnError(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Worker *instance;
      string error;
      if (!ValidateAndUnwrap(args, &instance, error))
      {
        cerr << "Worker::OnMessageError: " << error << endl;
        args.GetReturnValue().SetUndefined();
        return;
      }

      if (args.Length() < 1 || !args[0]->IsObject()) [[unlikely]]
      {
        cerr << "Worker::OnMessageError: Invalid arguments" << endl;
        args.GetReturnValue().SetUndefined();
        return;
      }

      instance->handle()->dispatchEvent(make_shared<dom::Event>(dom::DOMEventConstructorType::kEvent,
                                                                dom::DOMEventType::Error));
    }

    // static
    bool Worker::ValidateAndUnwrap(const FunctionCallbackInfo<Value> &args, Worker **pInstance, string &error)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (!args.This()->IsObject()) [[unlikely]]
      {
        error = "Invalid `this` object";
        return false;
      }

      Local<Object> workerImplObj = args.This()->ToObject(context).ToLocalChecked();
      Local<String> workerInstanceKey = String::NewFromUtf8(isolate, "_workerInstance").ToLocalChecked();
      Local<Value> workerValue;
      if (!workerImplObj->Get(context, workerInstanceKey).ToLocal(&workerValue) ||
          !Worker::IsInstanceOf(isolate, workerValue)) [[unlikely]]
      {
        error = "Invalid `_workerInstance` property";
        return false;
      }

      Local<Object> workerObj = workerValue.As<Object>();
      Worker *workerBinding = Worker::Unwrap(isolate, workerObj);
      if (workerBinding == nullptr) [[unlikely]]
      {
        error = "Failed to unwrap Worker instance";
        return false;
      }

      *pInstance = workerBinding;
      return true;
    }

    // static
    shared_ptr<dom::events::MessageEvent> Worker::CreateMessageEvent(Isolate *isolate, Local<Object> sourceObject)
    {
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Value> dataValue;
      if (!sourceObject->Get(context, String::NewFromUtf8(isolate, "data").ToLocalChecked())
             .ToLocal(&dataValue))
      {
        dataValue = Undefined(isolate);
      }

      string origin = "";
      Local<Value> originValue;
      if (!sourceObject->Get(context, String::NewFromUtf8(isolate, "origin").ToLocalChecked())
             .ToLocal(&originValue))
      {
        originValue = String::Empty(isolate);
      }
      else
      {
        String::Utf8Value originUtf8(isolate, originValue);
        origin = string(*originUtf8);
      }

      ValueSerializer dataSerializer(isolate);
      dataSerializer.WriteHeader();
      dataSerializer.WriteValue(context, dataValue).ToChecked();

      pair<uint8_t *, size_t> dataDescriptor = dataSerializer.Release();
      uint8_t *dataBuffer = dataDescriptor.first;
      size_t dataSize = dataDescriptor.second;

      return make_shared<dom::events::MessageEvent>(origin, dataBuffer, dataSize);
    }
  }
} // namespace endor
