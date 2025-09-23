#include "html_video_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    HTMLVideoElement::HTMLVideoElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : HTMLVideoElementBase(isolate, args)
    {
    }

    void HTMLVideoElement::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
    {
      v8::HandleScope handle_scope(isolate);
      auto instance_template = tpl->InstanceTemplate();
      auto prototype_template = tpl->PrototypeTemplate();

      // Video-specific properties
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                                     WidthGetter,
                                     WidthSetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                                     HeightGetter,
                                     HeightSetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "videoWidth").ToLocalChecked(),
                                     VideoWidthGetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "videoHeight").ToLocalChecked(),
                                     VideoHeightGetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "poster").ToLocalChecked(),
                                     PosterGetter,
                                     PosterSetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "playsInline").ToLocalChecked(),
                                     PlaysinlineGetter,
                                     PlaysinlineSetter);

      // Video-specific methods
      prototype_template->Set(v8::String::NewFromUtf8(isolate, "getVideoPlaybackQuality").ToLocalChecked(),
                              v8::FunctionTemplate::New(isolate, GetVideoPlaybackQuality));
      prototype_template->Set(v8::String::NewFromUtf8(isolate, "requestPictureInPicture").ToLocalChecked(),
                              v8::FunctionTemplate::New(isolate, RequestPictureInPicture));
    }

    v8::Local<v8::Object> HTMLVideoElement::NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::HTMLVideoElement> nativeElement)
    {
      v8::EscapableHandleScope handle_scope(isolate);
      auto context = isolate->GetCurrentContext();

      auto constructor = HTMLVideoElement::GetConstructorFunction(isolate);
      v8::Local<v8::Object> instance;

      if (constructor->NewInstance(context).ToLocal(&instance))
      {
        HTMLVideoElement::Wrap(isolate, instance, new HTMLVideoElement(isolate, v8::FunctionCallbackInfo<v8::Value>(nullptr, 0, nullptr)));
        // TODO: Set native element instance
      }

      return handle_scope.Escape(instance);
    }

    v8::Local<v8::Function> HTMLVideoElement::Initialize(v8::Isolate *isolate)
    {
      return HTMLVideoElementBase::Initialize(isolate);
    }

    // Video-specific property getters/setters
    void HTMLVideoElement::WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get width from native element
        info.GetReturnValue().Set(320); // Default video width
      }
    }

    void HTMLVideoElement::WidthSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element && value->IsNumber())
      {
        // TODO: Set width on native element
      }
    }

    void HTMLVideoElement::HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get height from native element
        info.GetReturnValue().Set(240); // Default video height
      }
    }

    void HTMLVideoElement::HeightSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element && value->IsNumber())
      {
        // TODO: Set height on native element
      }
    }

    void HTMLVideoElement::VideoWidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get intrinsic video width from native element
        info.GetReturnValue().Set(0); // 0 if no video loaded
      }
    }

    void HTMLVideoElement::VideoHeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get intrinsic video height from native element
        info.GetReturnValue().Set(0); // 0 if no video loaded
      }
    }

    void HTMLVideoElement::PosterGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get poster URL from native element
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    void HTMLVideoElement::PosterSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element && value->IsString())
      {
        // TODO: Set poster URL on native element
      }
    }

    void HTMLVideoElement::PlaysinlineGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get playsInline state from native element
        info.GetReturnValue().Set(false);
      }
    }

    void HTMLVideoElement::PlaysinlineSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Set playsInline state on native element
      }
    }

    // Video-specific methods
    void HTMLVideoElement::GetVideoPlaybackQuality(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Return VideoPlaybackQuality object
        info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
      }
    }

    void HTMLVideoElement::RequestPictureInPicture(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Return Promise for picture-in-picture mode
        info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
      }
    }
  }
}