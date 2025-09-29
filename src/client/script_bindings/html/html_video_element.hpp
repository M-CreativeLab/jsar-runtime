#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_media_element.hpp>
#include <client/html/html_video_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLVideoElement;
    using HTMLVideoElementBase = scripting_base::ObjectWrap<HTMLVideoElement,
                                                            dom::HTMLVideoElement,
                                                            HTMLMediaElement>;

    /**
     * HTMLVideoElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLVideoElement objects for use in V8 JavaScript execution contexts.
     * It provides the HTML video element interface extending HTMLMediaElement.
     */
    class HTMLVideoElement : public HTMLVideoElementBase
    {
      using HTMLVideoElementBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "HTMLVideoElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLVideoElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Video-specific properties
      static void WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void WidthSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HeightSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void VideoWidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void VideoHeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void PosterGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void PosterSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void PlaysinlineGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void PlaysinlineSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Video-specific methods
      static void GetVideoPlaybackQuality(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void RequestPictureInPicture(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}