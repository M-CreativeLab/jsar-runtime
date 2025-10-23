#include "./message_event.hpp"

namespace endor
{
  namespace script_bindings::event_bindings
  {
    using namespace std;
    using namespace v8;

    // static
    void MessageEvent::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      InstanceReadonlyAccessor(isolate, instance, "data", &MessageEvent::DataGetter);
      InstanceReadonlyAccessor(isolate, instance, "origin", &MessageEvent::OriginGetter);
      InstanceReadonlyAccessor(isolate, instance, "source", &MessageEvent::SourceGetter);
      InstanceReadonlyAccessor(isolate, instance, "ports", &MessageEvent::PortsGetter);
    }

    MessageEvent::MessageEvent(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : MessageEventBase(isolate, args)
    {
    }

    void MessageEvent::DataGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (!data_handle_.IsEmpty())
      {
        Local<Value> dataValue = data_handle_.Get(isolate);
        info.GetReturnValue().Set(dataValue);
        return;
      }
      else
      {
        const auto &messageData = handle()->data();
        if (messageData.empty())
        {
          info.GetReturnValue().SetNull();
        }
        else
        {
          ValueDeserializer deserializer(isolate, messageData.data(), messageData.size());
          Local<Context> context = isolate->GetCurrentContext();

          if (!deserializer.ReadHeader(context).ToChecked())
          {
            isolate->ThrowException(Exception::Error(
              MakeMethodError(isolate, "MessageEvent.data", "Failed to read message data header")));
            return;
          }

          Local<Value> dataValue;
          if (!deserializer.ReadValue(context).ToLocal(&dataValue))
          {
            isolate->ThrowException(Exception::Error(
              MakeMethodError(isolate, "MessageEvent.data", "Failed to read message data value")));
            return;
          }

          // Return the deserialized data value
          data_handle_.Reset(isolate, dataValue);
          info.GetReturnValue().Set(dataValue);
        }
      }
    }

    void MessageEvent::OriginGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->origin.c_str())
                                  .ToLocalChecked());
    }

    void MessageEvent::SourceGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().SetNull();
    }

    void MessageEvent::PortsGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }
  }
} // namespace endor
