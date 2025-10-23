#include <client/script_bindings/events/error_event.hpp>

namespace endor
{
  namespace script_bindings::event_bindings
  {
    using namespace std;
    using namespace v8;

    void ErrorEvent::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      InstanceReadonlyPropertyAccessor(isolate, prototype, "message", &ErrorEvent::MessageGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "filename", &ErrorEvent::FilenameGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "lineno", &ErrorEvent::LinenoGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "colno", &ErrorEvent::ColnoGetter);
      InstanceReadonlyPropertyAccessor(isolate, prototype, "error", &ErrorEvent::ErrorGetter);
    }

    ErrorEvent::ErrorEvent(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ErrorEventBase(isolate, args)
    {
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (args.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate, "requires at least 1 argument")));
        return;
      }

      string event_type;
      {
        String::Utf8Value utf8_value(isolate, args[0]);
        event_type = *utf8_value ? *utf8_value : "";
      }

      dom::events::ErrorEvent::Options options;
      if (args.Length() >= 2 && args[1]->IsObject())
      {
        Local<Object> options_object = args[1]->ToObject(context).ToLocalChecked();

        // Read the "message" property from the options object
        {
          Local<String> key = String::NewFromUtf8(isolate, "message").ToLocalChecked();
          if (options_object->Has(context, key).FromJust())
          {
            Local<Value> value = options_object->Get(context, key).ToLocalChecked();
            String::Utf8Value utf8_value(isolate, value);
            options.message = *utf8_value ? *utf8_value : "";
          }
        }

        // Read the "filename" property from the options object
        {
          Local<String> key = String::NewFromUtf8(isolate, "filename").ToLocalChecked();
          if (options_object->Has(context, key).FromJust())
          {
            Local<Value> value = options_object->Get(context, key).ToLocalChecked();
            String::Utf8Value utf8_value(isolate, value);
            options.filename = *utf8_value ? *utf8_value : "";
          }
        }

        // Read the "lineno" property from the options object
        {
          Local<String> key = String::NewFromUtf8(isolate, "lineno").ToLocalChecked();
          if (options_object->Has(context, key).FromJust())
          {
            Local<Value> value = options_object->Get(context, key).ToLocalChecked();
            if (value->IsNumber())
            {
              options.lineno = value->Uint32Value(context).FromJust();
            }
          }
        }

        // Read the "colno" property from the options object
        {
          Local<String> key = String::NewFromUtf8(isolate, "colno").ToLocalChecked();
          if (options_object->Has(context, key).FromJust())
          {
            Local<Value> value = options_object->Get(context, key).ToLocalChecked();
            if (value->IsNumber())
            {
              options.colno = value->Uint32Value(context).FromJust();
            }
          }
        }

        // Read the "error" property from the options object
        {
          Local<String> key = String::NewFromUtf8(isolate, "error").ToLocalChecked();
          if (options_object->Has(context, key).FromJust())
          {
            Local<Value> value = options_object->Get(context, key).ToLocalChecked();
            error_handle_.Reset(isolate, value);
          }
        }
      }

      setData(make_shared<dom::events::ErrorEvent>(event_type, options));
    }

    void ErrorEvent::MessageGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->message().c_str())
                                  .ToLocalChecked());
    }

    void ErrorEvent::FilenameGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->filename().c_str())
                                  .ToLocalChecked());
    }

    void ErrorEvent::LinenoGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Integer::New(isolate, handle()->lineno()));
    }

    void ErrorEvent::ColnoGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      args.GetReturnValue().Set(Integer::New(isolate, handle()->colno()));
    }

    void ErrorEvent::ErrorGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);

      if (!error_handle_.IsEmpty())
      {
        args.GetReturnValue().Set(error_handle_.Get(isolate));
      }
      else
      {
        args.GetReturnValue().SetNull();
      }
    }
  }
} // namespace endor
