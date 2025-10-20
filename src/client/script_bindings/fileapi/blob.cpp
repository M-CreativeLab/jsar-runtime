#include <client/script_bindings/fileapi/blob.hpp>

using namespace std;
using namespace v8;

namespace script_bindings::fileapi_bindings
{
  // static
  void Blob::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    InstanceReadonlyPropertyAccessor(isolate, prototype, "size", &Blob::SizeGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "type", &Blob::TypeGetter);

    InstanceMethod(isolate, prototype, "arrayBuffer", &Blob::ArrayBuffer);
    InstanceMethod(isolate, prototype, "bytes", &Blob::Bytes);
    InstanceMethod(isolate, prototype, "stream", &Blob::Stream);
    InstanceMethod(isolate, prototype, "text", &Blob::Text);
    InstanceMethod(isolate, prototype, "slice", &Blob::Slice);
  }

  Blob::Blob(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : BlobBase(isolate, args)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (args.Length() < 1 || !args[0]->IsArray())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeConstructorError(isolate, "First argument must be an array of blob parts")));
      return;
    }

    Local<Array> blobParts = Local<Array>::Cast(args[0]);
    vector<uint8_t> bytes;

    for (uint32_t i = 0; i < blobParts->Length(); ++i)
    {
      Local<Value> part = blobParts->Get(context, i).ToLocalChecked();

      if (part->IsString())
      {
        String::Utf8Value utf8(isolate, part);
        bytes.insert(bytes.end(), (*utf8), (*utf8) + utf8.length());
      }
      else if (part->IsArrayBuffer())
      {
        Local<v8::ArrayBuffer> arrayBuffer = Local<v8::ArrayBuffer>::Cast(part);
        auto backingStore = arrayBuffer->GetBackingStore();
        bytes.insert(bytes.end(),
                     static_cast<uint8_t *>(backingStore->Data()),
                     static_cast<uint8_t *>(backingStore->Data()) + backingStore->ByteLength());
      }
      else if (part->IsTypedArray())
      {
        Local<TypedArray> typedArray = Local<TypedArray>::Cast(part);
        Local<v8::ArrayBuffer> arrayBuffer = typedArray->Buffer();
        auto backingStore = arrayBuffer->GetBackingStore();
        bytes.insert(bytes.end(),
                     static_cast<uint8_t *>(backingStore->Data()) + typedArray->ByteOffset(),
                     static_cast<uint8_t *>(backingStore->Data()) + typedArray->ByteOffset() + typedArray->ByteLength());
      }
      else
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Unsupported blobPart type")
            .ToLocalChecked()));
        return;
      }
    }

    string type = "";
    if (args.Length() > 1 && args[1]->IsObject())
    {
      Local<Object> options = args[1]->ToObject(context).ToLocalChecked();
      Local<Value> typeValue = options->Get(context, String::NewFromUtf8(isolate, "type").ToLocalChecked())
                                 .ToLocalChecked();
      if (typeValue->IsString())
      {
        String::Utf8Value utf8(isolate, typeValue);
        type = *utf8;
      }
    }

    // Create the Blob object
    setData(make_shared<client_fileapi::Blob>(bytes, type));
  }

  void Blob::SizeGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    args.GetReturnValue().Set(Number::New(isolate,
                                          handle()->size()));
  }

  void Blob::TypeGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  handle()->type().c_str())
                                .ToLocalChecked());
  }

  void Blob::ArrayBuffer(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    auto result_resolver = Promise::Resolver::New(context).ToLocalChecked();
    args.GetReturnValue().Set(result_resolver->GetPromise());

    auto bytes_promise = handle()->bytes();
    auto bytes_future = bytes_promise.get_future();
    auto bytes_array = bytes_future.get();
    auto backing_store = ArrayBuffer::NewBackingStore(
      bytes_array.data(),
      bytes_array.size(),
      [](void *data, size_t length, void *deleter_data)
      {
        // No-op deleter since the data is owned by the Blob
      },
      nullptr);
    auto arraybuffer_value = ArrayBuffer::New(isolate, move(backing_store));
    result_resolver->Resolve(context, arraybuffer_value).ToChecked();
  }

  void Blob::Bytes(const FunctionCallbackInfo<Value> &args)
  {
    // Not implemented
  }

  void Blob::Stream(const FunctionCallbackInfo<Value> &args)
  {
    // Not implemented
  }

  void Blob::Text(const FunctionCallbackInfo<Value> &args)
  {
    // Not implemented
  }

  void Blob::Slice(const FunctionCallbackInfo<Value> &args)
  {
    // Not implemented
  }
}
