#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_media_element.hpp>
#include <client/html/html_video_element.hpp>

namespace endor
{
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
        void WidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void WidthSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void HeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void HeightSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void VideoWidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void VideoHeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void PosterGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void PosterSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void PlaysinlineGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void PlaysinlineSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

        // Video-specific methods
        void GetVideoPlaybackQuality(const v8::FunctionCallbackInfo<v8::Value> &info);
        void RequestPictureInPicture(const v8::FunctionCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor