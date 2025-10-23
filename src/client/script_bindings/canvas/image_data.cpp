#include "image_data.hpp"
#include <client/canvas/image_data.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace canvas_bindings
    {
      using namespace std;
      using namespace v8;

      void ImageData::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        Local<ObjectTemplate> instance = tpl->InstanceTemplate();

        // Properties (read-only)
        InstanceReadonlyAccessor(isolate, instance, "width", &ImageData::WidthGetter);
        InstanceReadonlyAccessor(isolate, instance, "height", &ImageData::HeightGetter);
        InstanceReadonlyAccessor(isolate, instance, "data", &ImageData::DataGetter);
        InstanceReadonlyAccessor(isolate, instance, "colorSpace", &ImageData::ColorSpaceGetter);
      }

      Local<Object> ImageData::NewInstance(Isolate *isolate, shared_ptr<::endor::canvas::ImageData> imageData)
      {
        EscapableHandleScope scope(isolate);
        assert(imageData != nullptr && "ImageData requires a valid native ImageData instance");
        return scope.Escape(ImageDataBase::NewInstance(isolate, imageData).As<Object>());
      }

      Local<Object> ImageData::NewInstance(Isolate *isolate, int width, int height, const string &colorSpace)
      {
        auto nativeImageData = make_shared<::endor::canvas::ImageData>(width, height, colorSpace);
        return NewInstance(isolate, nativeImageData);
      }

      ImageData::ImageData(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : ImageDataBase(isolate, args)
      {
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeConstructorError(isolate, "Requires at least 2 arguments")));
          return;
        }

        // TODO(yorkie): support colorSpace argument
        string colorSpace = "srgb";

        auto firstArg = args[0];
        if (firstArg->IsNumber() && args[1]->IsNumber())
        {
          int width = firstArg->ToNumber(context).ToLocalChecked()->Value();
          int height = args[1]->ToNumber(context).ToLocalChecked()->Value();
          setData(make_shared<::endor::canvas::ImageData>(width, height, colorSpace));
        }
        else if (firstArg->IsUint8ClampedArray() || firstArg->IsFloat32Array())
        {
          vector<char> dataArray;
          {
            auto data = firstArg.As<TypedArray>();
            auto buffer = data->Buffer();
            if (buffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeConstructorError(isolate, "Invalid Uint8ClampedArray or Float32Array")));
              return;
            }
            char *dataPtr = static_cast<char *>(buffer->GetBackingStore()->Data()) + data->ByteOffset();
            size_t length = data->ByteLength();
            dataArray.assign(dataPtr, dataPtr + length);
          }

          int width = args[1]->ToNumber(context).ToLocalChecked()->Value();
          if (width <= 0)
          {
            isolate->ThrowException(Exception::RangeError(
              String::NewFromUtf8Literal(isolate, "Width must be a positive number")));
            return;
          }

          int height;
          if (args.Length() >= 3 && args[2]->IsNumber())
          {
            height = args[2]->ToNumber(context).ToLocalChecked()->Value();
          }
          else
          {
            // Infer height from data length
            height = static_cast<int>(dataArray.size()) / (width * 4);
          }
          setData(make_shared<::endor::canvas::ImageData>(dataArray, width, height, colorSpace));
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeConstructorError(isolate, "Invalid argument types")));
          return;
        }
      }

      void ImageData::WidthGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Number::New(isolate, handle()->width()));
      }

      void ImageData::HeightGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Number::New(isolate, handle()->height()));
      }

      void ImageData::DataGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (data_value_handle_.IsEmpty())
        {
          void *data = handle()->addr();
          size_t size = handle()->computeByteSize();

          auto backing_deleter = [](void *data, size_t length, void *deleterData)
          {
            // Do nothing, as the memory is managed by `ImageData` class itself
          };
          shared_ptr<BackingStore> backing = ArrayBuffer::NewBackingStore(data, size, backing_deleter, nullptr);
          auto arrayBuffer = v8::ArrayBuffer::New(isolate, backing);

          // TODO(yorkie): Support Float32Array for rgba-float16
          auto uint8ClampedArray = Uint8ClampedArray::New(arrayBuffer, 0, size);
          data_value_handle_.Reset(isolate, uint8ClampedArray);
        }

        // Returns the cached typed array
        info.GetReturnValue().Set(data_value_handle_.Get(isolate));
      }

      void ImageData::ColorSpaceGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        const string &colorSpace = handle()->colorSpaceName();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, colorSpace.c_str()).ToLocalChecked());
      }
    }
  }
} // namespace endor
