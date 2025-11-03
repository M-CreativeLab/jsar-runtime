#pragma once

#include <memory>
#include <client/canvas/canvas.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/event_target.hpp>
#include <client/script_bindings/html/html_canvas_element.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace canvas_bindings
    {
      using Canvas = html_bindings::HTMLCanvasElement;

      class OffscreenCanvas;
      using OffscreenCanvasBase = scripting_base::ObjectWrap<OffscreenCanvas,
                                                             ::endor::canvas::OffscreenCanvas,
                                                             EventTarget>;

      /**
       * OffscreenCanvas wrapper for V8 objects using scripting_base::ObjectWrap.
       * 
       * This class wraps canvas::OffscreenCanvas objects for use in V8 JavaScript execution contexts.
       * It provides the standard OffscreenCanvas interface for off-screen rendering.
       */
      class OffscreenCanvas : public OffscreenCanvasBase
      {
        using OffscreenCanvasBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "OffscreenCanvas";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        OffscreenCanvas(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Property getters
        void WidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void HeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

        // Property setters
        void WidthSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void HeightSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

        // Methods
        void GetContext(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ConvertToBlob(const v8::FunctionCallbackInfo<v8::Value> &info);
        void TransferToImageBitmap(const v8::FunctionCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
