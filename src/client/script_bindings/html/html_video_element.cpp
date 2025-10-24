#include "html_video_element.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      HTMLVideoElement::HTMLVideoElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : HTMLVideoElementBase(isolate, args)
      {
      }

      void HTMLVideoElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope handle_scope(isolate);
        auto instance = tpl->InstanceTemplate();
        auto prototype = tpl->PrototypeTemplate();

        // Video-specific properties
        InstanceAccessor(isolate, instance, "width", &HTMLVideoElement::WidthGetter, &HTMLVideoElement::WidthSetter);
        InstanceAccessor(isolate, instance, "height", &HTMLVideoElement::HeightGetter, &HTMLVideoElement::HeightSetter);
        InstanceAccessor(isolate, instance, "videoWidth", &HTMLVideoElement::VideoWidthGetter, nullptr);
        InstanceAccessor(isolate, instance, "videoHeight", &HTMLVideoElement::VideoHeightGetter, nullptr);
        InstanceAccessor(isolate, instance, "poster", &HTMLVideoElement::PosterGetter, &HTMLVideoElement::PosterSetter);
        InstanceAccessor(isolate,
                         instance,
                         "playsInline",
                         &HTMLVideoElement::PlaysinlineGetter,
                         &HTMLVideoElement::PlaysinlineSetter);

        // Video-specific methods
        InstanceMethod(isolate, prototype, "getVideoPlaybackQuality", &HTMLVideoElement::GetVideoPlaybackQuality);
        InstanceMethod(isolate, prototype, "requestPictureInPicture", &HTMLVideoElement::RequestPictureInPicture);
      }

      // Video-specific property getters/setters
      void HTMLVideoElement::WidthGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Get width from native element
          info.GetReturnValue().Set(320); // Default video width
        }
      }

      void HTMLVideoElement::WidthSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element && value->IsNumber())
        {
          // TODO: Set width on native element
        }
      }

      void HTMLVideoElement::HeightGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Get height from native element
          info.GetReturnValue().Set(240); // Default video height
        }
      }

      void HTMLVideoElement::HeightSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element && value->IsNumber())
        {
          // TODO: Set height on native element
        }
      }

      void HTMLVideoElement::VideoWidthGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Get intrinsic video width from native element
          info.GetReturnValue().Set(0); // 0 if no video loaded
        }
      }

      void HTMLVideoElement::VideoHeightGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Get intrinsic video height from native element
          info.GetReturnValue().Set(0); // 0 if no video loaded
        }
      }

      void HTMLVideoElement::PosterGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Get poster URL from native element
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
        }
      }

      void HTMLVideoElement::PosterSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element && value->IsString())
        {
          // TODO: Set poster URL on native element
        }
      }

      void HTMLVideoElement::PlaysinlineGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Get playsInline state from native element
          info.GetReturnValue().Set(false);
        }
      }

      void HTMLVideoElement::PlaysinlineSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Set playsInline state on native element
        }
      }

      // Video-specific methods
      void HTMLVideoElement::GetVideoPlaybackQuality(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Return VideoPlaybackQuality object
          info.GetReturnValue().Set(Undefined(info.GetIsolate()));
        }
      }

      void HTMLVideoElement::RequestPictureInPicture(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        auto element = Unwrap(isolate, info.This());
        if (element)
        {
          // TODO: Return Promise for picture-in-picture mode
          info.GetReturnValue().Set(Undefined(info.GetIsolate()));
        }
      }
    }
  }
} // namespace endor
