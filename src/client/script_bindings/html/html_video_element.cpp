#include "html_video_element.hpp"

using namespace std;
using namespace v8;

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
      auto instance_template = tpl->InstanceTemplate();
      auto prototype_template = tpl->PrototypeTemplate();

      // Video-specific properties
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                                     WidthGetter,
                                     WidthSetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                                     HeightGetter,
                                     HeightSetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "videoWidth").ToLocalChecked(),
                                     VideoWidthGetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "videoHeight").ToLocalChecked(),
                                     VideoHeightGetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "poster").ToLocalChecked(),
                                     PosterGetter,
                                     PosterSetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "playsInline").ToLocalChecked(),
                                     PlaysinlineGetter,
                                     PlaysinlineSetter);

      // Video-specific methods
      prototype_template->Set(String::NewFromUtf8(isolate, "getVideoPlaybackQuality").ToLocalChecked(),
                              FunctionTemplate::New(isolate, GetVideoPlaybackQuality));
      prototype_template->Set(String::NewFromUtf8(isolate, "requestPictureInPicture").ToLocalChecked(),
                              FunctionTemplate::New(isolate, RequestPictureInPicture));
    }

    Local<Object> HTMLVideoElement::NewInstance(Isolate *isolate, shared_ptr<dom::HTMLVideoElement> nativeElement)
    {
      EscapableHandleScope scope(isolate);
      return nativeElement != nullptr
               ? scope.Escape(HTMLVideoElementBase::NewInstance(isolate, nativeElement).As<Object>())
               : scope.Escape(Local<Object>());
    }

    Local<Function> HTMLVideoElement::Initialize(Isolate *isolate)
    {
      return HTMLVideoElementBase::Initialize(isolate);
    }

    // Video-specific property getters/setters
    void HTMLVideoElement::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get width from native element
        info.GetReturnValue().Set(320); // Default video width
      }
    }

    void HTMLVideoElement::WidthSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element && value->IsNumber())
      {
        // TODO: Set width on native element
      }
    }

    void HTMLVideoElement::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get height from native element
        info.GetReturnValue().Set(240); // Default video height
      }
    }

    void HTMLVideoElement::HeightSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element && value->IsNumber())
      {
        // TODO: Set height on native element
      }
    }

    void HTMLVideoElement::VideoWidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get intrinsic video width from native element
        info.GetReturnValue().Set(0); // 0 if no video loaded
      }
    }

    void HTMLVideoElement::VideoHeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get intrinsic video height from native element
        info.GetReturnValue().Set(0); // 0 if no video loaded
      }
    }

    void HTMLVideoElement::PosterGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get poster URL from native element
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    void HTMLVideoElement::PosterSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element && value->IsString())
      {
        // TODO: Set poster URL on native element
      }
    }

    void HTMLVideoElement::PlaysinlineGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get playsInline state from native element
        info.GetReturnValue().Set(false);
      }
    }

    void HTMLVideoElement::PlaysinlineSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
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
